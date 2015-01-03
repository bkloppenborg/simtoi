/*
 * wAnimation.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wModels.h"

#include "CGLWidget.h"
#include "guiModel.h"

#include "CTreeModel.h"
#include "CParameterItem.h"
#include "CModel.h"
#include "CModelList.h"
#include "CFeature.h"

wModels::wModels(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);
}

wModels::~wModels()
{

}

void wModels::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

	// connect any non-automatic signal/slots
	connect(&mTreeModel, SIGNAL(parameterUpdated()), mGLWidget.get(), SLOT(on_mTreeModel_parameterUpdated()));
	connect(mGLWidget.get(), SIGNAL(modelUpdated()), this, SLOT(on_modelUpdated()));

	ButtonCheck();
	TreeCheck();
}

void wModels::on_btnAddModel_clicked(void)
{
	if(!mGLWidget)
		return;

    int id = 0;
    int n_features;

    guiModel tmp;
    tmp.show();

    if(tmp.exec())
    {
    	mGLWidget->addModel(tmp.getModel());
    }

    ButtonCheck();
    TreeCheck();
}

/// Deletes the selected model from the model list
void wModels::on_btnRemoveModel_clicked(void)
{
	if(!mGLWidget)
		return;

	unsigned int index = 0;
	mGLWidget->removeModel(index);
	mGLWidget->Render();

	ButtonCheck();
    TreeCheck();
}

/// Opens up an editing dialog for the currently selected model.
void wModels::on_btnEditModel_clicked()
{
	if(!mGLWidget)
		return;

	unsigned int old_model_index = 0;
	CModelPtr old_model = mGLWidget->getModel(old_model_index);

	guiModel dialog(old_model);
	if(dialog.exec())
	{
		CModelPtr new_model = dialog.getModel();
		mGLWidget->replaceModel(old_model_index, new_model);
	}

	mGLWidget->Render();

	ButtonCheck();
    TreeCheck();
}

void wModels::on_modelUpdated()
{
	RebuildTree();
	TreeCheck();
}

void wModels::ButtonCheck()
{
	if(!mGLWidget)
		return;

	this->btnAddModel->setEnabled(false);
	this->btnEditModel->setEnabled(false);
	this->btnRemoveModel->setEnabled(false);

	// Buttons for add/edit/delete model
	this->btnAddModel->setEnabled(true);
	if(mTreeModel.rowCount() > 0)
	{
		this->btnEditModel->setEnabled(true);
		this->btnRemoveModel->setEnabled(true);
	}
}

void wModels::TreeCheck()
{
	this->treeModels->setHeaderHidden(false);
	this->treeModels->setModel(&mTreeModel);
	this->treeModels->header()->setResizeMode(QHeaderView::ResizeToContents);
	// expand the tree fully
	this->treeModels->expandAll();
}

void wModels::LoadParameters(QStandardItem * parent_widget, CParameterMap * param_map)
{
	// Get a reference to the map.
	const map<string, CParameter> parameter_map = param_map->getParameterMap();

	for(auto id_parameter: parameter_map)
	{
		const string parameter_id = id_parameter.first;
		const CParameter parameter = id_parameter.second;

		QList<QStandardItem *> items;
		QStandardItem * item;

		// First the name
		item = new QStandardItem(QString::fromStdString( parameter.getHumanName() ));
		items << item;

		// Now the checkbox
		item = new CParameterItem(param_map, parameter_id);
		item->setEditable(true);
		item->setCheckable(true);
		if(parameter.isFree())
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		items << item;

		// The Value
		item = new CParameterItem(param_map, parameter_id);
		item->setEditable(true);
		item->setData(QVariant( parameter.getValue() ), Qt::DisplayRole);
		items << item;

		// Minimum parameter value
		item = new CParameterItem(param_map, parameter_id);
		item->setEditable(true);
		item->setData(QVariant( parameter.getMin() ), Qt::DisplayRole);
		items << item;

		// Maximum parameter value
		item = new CParameterItem(param_map, parameter_id);
		item->setEditable(true);
		item->setData(QVariant( parameter.getMax() ), Qt::DisplayRole);
		items << item;

		// Maximum step size
		item = new CParameterItem(param_map, parameter_id);
		item->setEditable(true);
		item->setData(QVariant( parameter.getStepSize()), Qt::DisplayRole);
		items << item;

		parent_widget->appendRow(items);
	}
}

QList<QStandardItem *> wModels::LoadParametersHeader(QString name, CParameterMap * param_map)
{
	QList<QStandardItem *> items;
	QStandardItem * item;
	item = new QStandardItem(name);
	items << item;
	item = new QStandardItem(QString(""));
	items << item;
	item = new QStandardItem(QString::fromStdString(param_map->getName()));
	items << item;

	return items;
}

void wModels::RebuildTree()
{
	QStringList labels = QStringList();
	labels << "Name" << "Free" << "Value" << "Min" << "Max" << "Step";
	mTreeModel.clear();
	mTreeModel.setColumnCount(5);
	mTreeModel.setHorizontalHeaderLabels(labels);
	CModelListPtr model_list = mGLWidget->getModels();

	QList<QStandardItem *> items;
	QStandardItem * item;
	QStandardItem * item_parent;
	shared_ptr<CModel> model;
	shared_ptr<CPosition> position;
	CShader * shader;

	// Now pull out the pertinent information
	// NOTE: We drop the shared_ptrs here
	// TODO: Propigate shared pointers
	for(int i = 0; i < model_list->size(); i++)
	{
		// First pull out the model parameters
		model = model_list->GetModel(i);

		items = wModels::LoadParametersHeader(QString("Model"), model.get());
		item_parent = items[0];
		mTreeModel.appendRow(items);
		wModels::LoadParameters(item_parent, model.get());

		// Now for the Position Parameters
		position = model->GetPosition();
		items = wModels::LoadParametersHeader(QString("Position"), position.get());
		item = items[0];
		item_parent->appendRow(items);
		wModels::LoadParameters(item, position.get());

		// Lastly for the shader:
		shader = model->GetShader().get();
		items = wModels::LoadParametersHeader(QString("Shader"), shader);
		item = items[0];
		item_parent->appendRow(items);
		wModels::LoadParameters(item, shader);

		auto features = model->GetFeatures();
		for(auto feature: features)
		{
			items = wModels::LoadParametersHeader(QString("Feature"), feature.get());
			item = items[0];
			item_parent->appendRow(items);
			wModels::LoadParameters(item, feature.get());
		}
	}
}
