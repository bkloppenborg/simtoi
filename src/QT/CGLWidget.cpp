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
#include "CDataInfo.h"

// Temporary includes while migrating code to wModels
#include "wParameterEditor.h"

extern string EXE_FOLDER;

CGLWidget::CGLWidget(QWidget * widget_parent, string shader_source_dir, string cl_kernel_dir)
    : QGLWidget(widget_parent)
{ 
	// Shut off auto buffer swapping and call doneCurrent to release the OpenGL context
	setAutoBufferSwap(false);
    this->doneCurrent();

    // Immediately initialize the worker thread. This will claim the OPenGL context.
	mWorker.reset(new CWorkerThread(this, QString::fromStdString(EXE_FOLDER)));

	mSaveDirectory = "";
}

CGLWidget::~CGLWidget()
{
//	stopMinimizer();
	stopRendering();
}

void CGLWidget::addData(string filename)
{
	CDataInfo temp = mWorker->addData(filename);
	emit dataAdded(temp);
}

void CGLWidget::addModel(CModelPtr model)
{
	mWorker->addModel(model);
	emit modelUpdated();
}

CModelPtr CGLWidget::getModel(unsigned int model_index)
{
	return mWorker->getModel(model_index);
}

void CGLWidget::replaceModel(unsigned int model_index, CModelPtr new_model)
{
	mWorker->replaceModel(model_index, new_model);
	emit modelUpdated();
}

void CGLWidget::removeData(unsigned int data_index)
{
//	mWorker->removeData(data_index);
//	emit dataRemoved(data_index);
}

void CGLWidget::removeModel(unsigned int model_index)
{
	mWorker->removeModel(model_index);
	emit modelUpdated();
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


/// Returns a list of file filters for use in QFileDialog
QStringList CGLWidget::GetFileFilters()
{
	return mWorker->GetFileFilters();
}

double CGLWidget::GetTime()
{
	return mWorker->GetTime();
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

	emit modelUpdated();
}

void CGLWidget::paintEvent(QPaintEvent *)
{
    mWorker->Render();
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

void CGLWidget::startRendering()
{
	// Tell the thread to start.
    mWorker->start();
}

void CGLWidget::stopRendering()
{
    mWorker->stop();
}


//void CGLWidget::updateData(void)
//{
//	emit dataUpdated();
//}

void CGLWidget::updateParameters()
{
	mWorker->Render();
}
