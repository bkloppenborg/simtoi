/*
 * wAnimation.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wModels.h"

#include "CGLWidget.h"
#include "guiModel.h"

wModels::wModels(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);
}

wModels::wModels(CGLWidgetPtr gl_widget, QWidget * parent)
	: QWidget(parent)
{
	mGLWidget = gl_widget;
	this->setupUi(this);
}

wModels::~wModels()
{

}

void wModels::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;
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

void wModels::ButtonCheck()
{
	if(!mGLWidget)
		return;

	this->btnAddModel->setEnabled(false);
	this->btnEditModel->setEnabled(false);
	this->btnRemoveModel->setEnabled(false);

	// Buttons for add/edit/delete model
	this->btnAddModel->setEnabled(true);
	if(mGLWidget->GetTreeModel()->rowCount() > 0)
	{
		this->btnEditModel->setEnabled(true);
		this->btnRemoveModel->setEnabled(true);
	}
}

void wModels::TreeCheck()
{
	if(!mGLWidget)
		return;

	// Configure the model tree
    CTreeModel * model = mGLWidget->GetTreeModel();
	this->treeModels->setHeaderHidden(false);
	this->treeModels->setModel(mGLWidget->GetTreeModel());
	this->treeModels->header()->setResizeMode(QHeaderView::ResizeToContents);
	// expand the tree fully
	this->treeModels->expandAll();
}
