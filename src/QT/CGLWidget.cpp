/* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CGLWidget.h"
#include "CTreeModel.h"

#include "CMinimizer.h"

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
	QStringList labels = QStringList();
	labels << "File" << "Mean JD";
	mOpenFileModel->clear();
	mOpenFileModel->setColumnCount(2);
	mOpenFileModel->setHorizontalHeaderLabels(labels);

    mTreeModel = new CTreeModel();
}

CGLWidget::~CGLWidget()
{
	stopRendering();
	delete mOpenFileModel;
	delete mTreeModel;
}

void CGLWidget::AddModel(CModelList::ModelTypes model_type)
{
	// Instruct the thread to add the model to it's list:
	mGLT.AddModel(model_type);

	RebuildTree();
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
	StopMinimizer();
    stopRendering();
    QGLWidget::closeEvent(evt);
}

void CGLWidget::EnqueueOperation(CL_GLT_Operations op)
{
	mGLT.EnqueueOperation(op);
}

void CGLWidget::LoadParameters(QStandardItem * parent_widget, CParameters * parameters)
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

		parent_widget->appendRow(items);
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

void CGLWidget::LoadMinimizer(CMinimizer::MinimizerTypes minimizer_type)
{
	// TODO: Permit loading of different minimizers here.
	CMinimizer * tmp = CMinimizer::GetMinimizer(minimizer_type, &mGLT);
	mMinThread.SetMinimizer(tmp);
}

void CGLWidget::MinimizerExit(void)
{
	emit( MinimizationFinished(dynamic_cast<QWidget*>(parent()) ) );
}

void CGLWidget::Open(string filename)
{
	mGLT.Open(filename);
	RebuildTree();
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
	// NOTE: We drop the shared_ptrs here
	// TODO: Propigate shared pointers
	for(int i = 0; i < model_list->size(); i++)
	{
		// First pull out the model parameters
		model = model_list->GetModel(i).get();

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
		shader = model->GetShader().get();
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

	connect(&mMinThread, SIGNAL(finished(void)), this, SLOT(MinimizerExit(void)));
}

void CGLWidget::startRendering()
{
	// tell the thread to start
    mGLT.start();

    // wait for the thread to be initialized before proceeding
    // NOTE: if thread initialization fails, this could lead to deadlock!
    while(!mGLT.IsRunning())
    	usleep(5000);

}

void CGLWidget::stopRendering()
{
    mGLT.stop();
    mGLT.wait();
}

void CGLWidget::SetFreeParameters(double * params, int n_params, bool scale_params)
{
	mGLT.SetFreeParameters(params, n_params, scale_params);
}

void CGLWidget::SetScale(double scale)
{
	mGLT.SetScale(scale);
}

void CGLWidget::SetShader(int model_id, CGLShaderList::ShaderTypes shader)
{
	mGLT.SetShader(model_id, shader);
	RebuildTree();
}

void CGLWidget::SetPositionType(int model_id, CPosition::PositionTypes pos_type)
{
	mGLT.SetPositionType(model_id, pos_type);
	RebuildTree();
}

void CGLWidget::StopMinimizer()
{
	mMinThread.stop();
}
