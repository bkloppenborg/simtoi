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

#include <QMdiSubWindow>
#include <stdexcept>
#include "textio.hpp"
#include "json/json.h"
using namespace std;


#include "CTreeModel.h"
#include "CAnimator.h"
#include "CMinimizerThread.h"
#include "CAnimator.h"
#include "CModel.h"
#include "CModelList.h"
#include "CTreeModel.h"
#include "CParameterItem.h"
#include "CFeature.h"

extern string EXE_FOLDER;

CGLWidget::CGLWidget(QWidget * widget_parent, string shader_source_dir, string cl_kernel_dir)
    : QGLWidget(widget_parent)
{ 
	// Shut off auto buffer swapping and call doneCurrent to release the OpenGL context
	setAutoBufferSwap(false);
    this->doneCurrent();

    // Immediately initialize the worker thread. This will claim the OPenGL context.
	mWorker.reset(new CWorkerThread(this, QString::fromStdString(EXE_FOLDER)));

	// Create the animation thread:
	mAnimator.reset(new CAnimator(this));
	mAnimator->moveToThread(&mAnimationThread);
	connect(this, SIGNAL(startAnimation(double, double)), mAnimator.get(), SLOT(start_animation(double, double)));
//	connect(this, SIGNAL(stopAnimation(void)), mAnimator.get(), SLOT(stop_animation(void)));
	mAnimationThread.start();

	QStringList labels = QStringList();
	labels << "File" << "Mean JD";
	mOpenFileModel.clear();
	mOpenFileModel.setColumnCount(2);
	mOpenFileModel.setHorizontalHeaderLabels(labels);

	// This signal-slot would not connect automatically, so we do it explicitly here.
	connect(&mTreeModel, SIGNAL(parameterUpdated()), this, SLOT(on_mTreeModel_parameterUpdated()));

	mSaveDirectory = "";
}

CGLWidget::~CGLWidget()
{
	// Stop any running threads
	StopAnimation();
	mAnimationThread.quit();
	mAnimationThread.wait();

	stopMinimizer();
	stopRendering();
}

void CGLWidget::AddModel(shared_ptr<CModel> model)
{
	// Instruct the thread to add the model to it's list:
	mWorker->AddModel(model);

	RebuildTree();
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
    stopRendering();
    QGLWidget::closeEvent(evt);
}

void CGLWidget::Export(QString save_folder)
{
	mWorker->ExportResults(save_folder);
}

//void CGLWidget::LoadParameters(QStandardItem * parent_widget, CParameters * parameters)
//{
//	for(int j = 0; j < parameters->GetNParams(); j++)
//	{
//		QList<QStandardItem *> items;
//		QStandardItem * item;
//
//		// First the name
//		item = new QStandardItem(QString::fromStdString(parameters->GetParamName(j)));
//		items << item;
//
//		// Now the checkbox
//		item = new CParameterItem(parameters, j);
//		item->setEditable(true);
//		item->setCheckable(true);
//		if(parameters->IsFree(j))
//			item->setCheckState(Qt::Checked);
//		else
//			item->setCheckState(Qt::Unchecked);
//		items << item;
//
//		// The Value
//		item = new CParameterItem(parameters, j);
//		item->setEditable(true);
//		item->setData(QVariant((double)parameters->GetParam(j)), Qt::DisplayRole);
//		items << item;
//
//		// Minimum parameter value
//		item = new CParameterItem(parameters, j);
//		item->setEditable(true);
//		item->setData(QVariant((double)parameters->GetMin(j)), Qt::DisplayRole);
//		items << item;
//
//		// Maximum parameter value
//		item = new CParameterItem(parameters, j);
//		item->setEditable(true);
//		item->setData(QVariant((double)parameters->GetMax(j)), Qt::DisplayRole);
//		items << item;
//
//		// Maximum step size
//		item = new CParameterItem(parameters, j);
//		item->setEditable(true);
//		item->setData(QVariant((double)parameters->GetStepSize(j)), Qt::DisplayRole);
//		items << item;
//
//		parent_widget->appendRow(items);
//	}
//}

void CGLWidget::LoadParameters(QStandardItem * parent_widget, CParameterMap * param_map)
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


//QList<QStandardItem *> CGLWidget::LoadParametersHeader(QString name, CParameters * param_base)
//{
//	QList<QStandardItem *> items;
//	QStandardItem * item;
//	item = new QStandardItem(name);
//	items << item;
//	item = new QStandardItem(QString(""));
//	items << item;
//	item = new QStandardItem(QString::fromStdString(param_base->GetName()));
//	items << item;
//
//	return items;
//}

QList<QStandardItem *> CGLWidget::LoadParametersHeader(QString name, CParameterMap * param_map)
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

/// Returns a list of file filters for use in QFileDialog
QStringList CGLWidget::GetFileFilters()
{
	return mWorker->GetFileFilters();
}

/// Returns the Minimizer's ID if one is loaded, otherwise an empty string.
string CGLWidget::GetMinimizerID()
{
	if(mMinimizer)
		return mMinimizer->GetID();

	return "";
}

bool CGLWidget::GetMinimizerRunning()
{
	if(!mMinimizer)
		return false;

	return mMinimizer->isRunning();
}

double CGLWidget::GetTime()
{
	return mWorker->GetTime();
}

bool CGLWidget::IsAnimating()
{
	return mAnimator->IsRunning();
}

void CGLWidget::on_mTreeModel_parameterUpdated()
{
	mWorker->Render();
}

void CGLWidget::on_minimizer_finished(void)
{
	emit minimizerFinished();
}

void CGLWidget::Open(string filename)
{
	Json::Reader reader;
	Json::Value input;
	string file_contents = ReadFile(filename, "Could not read SIMTOI save file: '" + filename + "'. Does the file exist?");
	bool parsingSuccessful = reader.parse(file_contents, input);
	if(!parsingSuccessful)
	{
		string error_message = reader.getFormatedErrorMessages();

		throw runtime_error("Could not parse SIMTOI configuration file: \n'"
				+ filename + "'\n"
				+ "The error message follows: \n"
				+ error_message);
	}

	int width = input["area_width"].asInt();
	int height = input["area_height"].asInt();
	double scale = input["area_scale"].asDouble();

	// If the width and height are nonsense, override them.
	if(width < 1 || height < 1)
	{
		width = 128;
		height = 128;
	}

	// Set the area scale and height
	mWorker->SetSize(width, height);
	mWorker->SetScale(scale);

	// Now have the Worker thread open the remainder of the file.
	mWorker->Restore(input);

	RebuildTree();
}

void CGLWidget::OpenData(string filename)
{
	mWorker->OpenData(filename);
}

void CGLWidget::paintEvent(QPaintEvent *)
{
    mWorker->Render();
}

void CGLWidget::RebuildTree()
{
	QStringList labels = QStringList();
	labels << "Name" << "Free" << "Value" << "Min" << "Max" << "Step";
	mTreeModel.clear();
	mTreeModel.setColumnCount(5);
	mTreeModel.setHorizontalHeaderLabels(labels);
	CModelListPtr model_list = mWorker->GetModelList();

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

		items = LoadParametersHeader(QString("Model"), model.get());
		item_parent = items[0];
		mTreeModel.appendRow(items);
		LoadParameters(item_parent, model.get());

		// Now for the Position Parameters
		position = model->GetPosition();
		items = LoadParametersHeader(QString("Position"), position.get());
		item = items[0];
		item_parent->appendRow(items);
		LoadParameters(item, position.get());

		// Lastly for the shader:
		shader = model->GetShader().get();
		items = LoadParametersHeader(QString("Shader"), shader);
		item = items[0];
		item_parent->appendRow(items);
		LoadParameters(item, shader);

		auto features = model->GetFeatures();
		for(auto feature: features)
		{
			items = LoadParametersHeader(QString("Feature"), feature.get());
			item = items[0];
			item_parent->appendRow(items);
			LoadParameters(item, feature.get());
		}
	}
}

void CGLWidget::Render()
{
	mWorker->Render();
}

void CGLWidget::resizeEvent(QResizeEvent *evt)
{
	// do nothing, the area cannot be resized once created
}

void CGLWidget::Save(string filename)
{
	Json::StyledStreamWriter writer;
	Json::Value output;

	// Serialize the mWorker object first
	output = mWorker->Serialize();
	// Add a comment that provides some information about what wrote the file.
	output["_file_info"] = "Save file from the SImulation and Modeling Tool for "
			"Optical Interferometry (SIMTOI). See "
			"https://github.com/bkloppenborg/simtoi for information about this "
			"format.";

	// Now save the OpenGL area information.
	output["area_width"] = mWorker->GetImageWidth();
	output["area_height"] = mWorker->GetImageHeight();
	output["area_scale"] = mWorker->GetImageScale();

	std::ofstream outfile(filename.c_str());
	writer.write(outfile, output);
}

void CGLWidget::SetScale(double scale)
{
	mWorker->SetScale(scale);
}

void CGLWidget::SetMinimizer(CMinimizerPtr minimizer)
{
	stopMinimizer();
	mMinimizer = minimizer;
	mMinimizer->Init(mWorker);
	mMinimizer->SetSaveDirectory(mSaveDirectory);
}

void CGLWidget::SetSaveDirectory(string directory_path)
{
	mSaveDirectory = directory_path;
}

void CGLWidget::SetSize(unsigned int width, unsigned int height)
{
	mWorker->SetSize(width, height);
}

void CGLWidget::SetTime(double time)
{
	mWorker->SetTime(time);
}

void CGLWidget::StartAnimation(double start_time, double time_step)
{
	emit(startAnimation(start_time, time_step));
}

void CGLWidget::StopAnimation()
{
	mAnimator->mRun = false;
}

void CGLWidget::startMinimizer()
{
	if(!mMinimizer)
		return;

	// Stop the animation, if it is running
	StopAnimation();

	mMinimizer->start();
	connect(mMinimizer.get(), SIGNAL(finished()), this, SLOT(on_minimizer_finished(void)));
}

void CGLWidget::stopMinimizer()
{
	if(!mMinimizer)
		return;

	// Stop the thread. If it was running, join it.
	mMinimizer->stop();
}

void CGLWidget::startRendering()
{
	// Tell the thread to start.
    mWorker->start();
}

void CGLWidget::stopRendering()
{
    mWorker->stop();
}

