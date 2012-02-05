#include "CGLWidget.h"
#include "CTreeModel.h"

#include "CMinimizer.h"
#include "CMinimizer_mpfit.h"

#include "CModel.h"
#include "CModelList.h"
#include "CTreeModel.h"
#include "CParameterItem.h"


CGLWidget::CGLWidget(QWidget * widget_parent, string shader_source_dir, string cl_kernel_dir)
    : QGLWidget(widget_parent), mGLT(this, shader_source_dir, cl_kernel_dir), mMinThread()
{ 
    setAutoBufferSwap(false);
    this->doneCurrent();

    mOpenFileModel = new QStandardItemModel();
    mTreeModel = new CTreeModel();
}

CGLWidget::~CGLWidget()
{
	stopRendering();
	delete mOpenFileModel;
	delete mTreeModel;
}

void CGLWidget::AddModel(eModels model_type)
{
	// Instruct the thread to add the model to it's list:
	mGLT.AddModel(model_type);

	RebuildTree();
}

void CGLWidget::EnqueueOperation(CL_GLT_Operations op)
{
	mGLT.EnqueueOperation(op);
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
    stopRendering();
    QGLWidget::closeEvent(evt);
}

void CGLWidget::LoadParameters(QStandardItem * parent, CParameters * parameters)
{
	for(int j = 0; j < parameters->GetNParams(); j++)
	{
		QList<QStandardItem *> items;
		QStandardItem * item;

		// First the name
		item = new QStandardItem(QString::fromStdString(parameters->GetParamName(j)));
		items << item;

		// Now the checkbox
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setCheckable(true);
		if(parameters->IsFree(j))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		items << item;

		// The Value
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setData(QVariant((double)parameters->GetParam(j)), Qt::DisplayRole);
		items << item;

		// Minimum parameter value
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setData(QVariant((double)parameters->GetMin(j)), Qt::DisplayRole);
		items << item;

		// Maximum parameter value
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setData(QVariant((double)parameters->GetMax(j)), Qt::DisplayRole);
		items << item;

		parent->appendRow(items);
	}
}

QList<QStandardItem *> CGLWidget::LoadParametersHeader(QString name, CParameters * param_base)
{
	QList<QStandardItem *> items;
	QStandardItem * item;
	item = new QStandardItem(name);
	items << item;
	item = new QStandardItem(QString(""));
	items << item;
	item = new QStandardItem(QString::fromStdString(param_base->GetName()));
	items << item;

	return items;
}

void CGLWidget::LoadMinimizer()
{
	// TODO: Permit loading of different minimizers here.
	CMinimizer * tmp;
	tmp = (CMinimizer*) new CMinimizer_mpfit(&mGLT);
	mMinThread.SetMinimizer(tmp);
}

void CGLWidget::paintEvent(QPaintEvent *)
{
    mGLT.EnqueueOperation(GLT_RenderModels);
}

void CGLWidget::RebuildTree()
{
	QStringList labels = QStringList();
	labels << "Name" << "Free" << "Value" << "Min" << "Max";
	mTreeModel->clear();
	mTreeModel->setColumnCount(5);
	mTreeModel->setHorizontalHeaderLabels(labels);
	CModelList * model_list = mGLT.GetModelList();

	QList<QStandardItem *> items;
	QStandardItem * item;
	QStandardItem * item_parent;
	CModel * model;
	CPosition * position;
	CGLShaderWrapper * shader;

	// Now pull out the pertinent information
	for(int i = 0; i < model_list->size(); i++)
	{
		// First pull out the model parameters
		model = model_list->GetModel(i);
		items = LoadParametersHeader(QString("Model"), model);
		item_parent = items[0];
		mTreeModel->appendRow(items);
		LoadParameters(item_parent, model);

		// Now for the Position Parameters
		position = model->GetPosition();
		items = LoadParametersHeader(QString("Position"), position);
		item = items[0];
		item_parent->appendRow(items);
		LoadParameters(item, position);

		// Lastly for the shader:
		shader = model->GetShader();
		items = LoadParametersHeader(QString("Shader"), shader);
		item = items[0];
		item_parent->appendRow(items);
		LoadParameters(item, shader);
	}


}

void CGLWidget::resizeEvent(QResizeEvent *evt)
{
    mGLT.resizeViewport(evt->size());
}

void CGLWidget::RunMinimizer()
{
	mMinThread.start();
}

void CGLWidget::startRendering()
{
    mGLT.start();
}

void CGLWidget::stopRendering()
{
    mGLT.stop();
    mGLT.wait();
}

void CGLWidget::SetFreeParameters(float * params, int n_params)
{
	mGLT.SetFreeParameters(params, n_params);
}

void CGLWidget::SetScale(double scale)
{
	mGLT.SetScale(scale);
}

void CGLWidget::SetShader(int model_id, eGLShaders shader)
{
	mGLT.SetShader(model_id, shader);
	RebuildTree();
}

void CGLWidget::SetPositionType(int model_id, ePositionTypes pos_type)
{
	mGLT.SetPositionType(model_id, pos_type);
	RebuildTree();
}

