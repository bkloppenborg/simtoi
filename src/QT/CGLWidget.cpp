 /*
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
 *
 * Copyright (c) 2012, 2014 Brian Kloppenborg
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
#include "../version.h"

extern string EXE_FOLDER;

CGLWidget::CGLWidget(QWidget * widget_parent)
    : QGLWidget(widget_parent)
{ 
	// Reset the worker thread
	mWorker = make_shared<CWorkerThread>(this, QString::fromStdString(EXE_FOLDER));

	mSaveDirectory = "";

	connect(mWorker.get(), SIGNAL(glContextWarning(string)), this, SLOT(receiveWarning(string)));
}

CGLWidget::~CGLWidget()
{
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

bool CGLWidget::checkExtensionAvailability(string ext_name)
{
    bool ret_val = false;

    // get the total number of extensions
    GLint num_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

    // Now query for each extensions
    for(int i = 0; i < num_extensions; i++)
    {
    	const GLubyte * gl_extension = glGetStringi(GL_EXTENSIONS, i);
        const char * gl_extension_c = reinterpret_cast<const char*>(gl_extension);
        string ext = string(gl_extension_c);

        if(ext == ext_name)
        {
        	ret_val = true;
        	break;
        }
    }

    return ret_val;
}

CModelPtr CGLWidget::getModel(unsigned int model_index)
{
	return mWorker->getModel(model_index);
}

void CGLWidget::removeData(unsigned int data_index)
{
	mWorker->removeData(data_index);
	emit dataRemoved(data_index);
}

void CGLWidget::removeModel(unsigned int model_index)
{
	mWorker->removeModel(model_index);
	emit modelUpdated();
}

void CGLWidget::replaceModel(unsigned int model_index, CModelPtr new_model)
{
	mWorker->replaceModel(model_index, new_model);
	emit modelUpdated();
}

void CGLWidget::resetWidget()
{
	// remove the data from the GUI
	for(int i = mWorker->GetNDataFiles(); i > -1; i--)
	{
		cout << "Instructing removal of " << i << endl;
		emit dataRemoved(i);
	}

	mWorker = make_shared<CWorkerThread>(this, QString::fromStdString(EXE_FOLDER));

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
	resetWidget();
	SetSize(width, height);
	SetScale(scale);

	// Now have the Worker thread open the remainder of the file.
	mWorker->Restore(input);

	emit modelUpdated();
}

//void CGLWidget::paintEvent(QPaintEvent * )
//{
//	Render();
//}

/// Override the QGLWidget::glDraw function when the worker thread is running.
void CGLWidget::glDraw()
{
	// If the worker is not running, we render using the default glDraw function
	// (which eventually calls paintGL).
	if(!mWorker->isRunning())
	{
		QGLWidget::glDraw();
	}
	else
	{
		Render();
	}
}

/// Renders to the default OpenGL framebuffer
void CGLWidget::paintGL()
{
	// Until the worker thread is running, we render a blank gray window.
	if(!mWorker->isRunning())
	{
		glClearColor(0.5, 0.5, 0.5, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void CGLWidget::Render()
{
	mWorker->Render();
}

void CGLWidget::receiveWarning(string message)
{
	emit warning(message);
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
	stringstream header;
	header << "Save file from the SImulation and Modeling Tool for Optical "
		   << "Interferometry (SIMTOI) version " << SIMTOI_VERSION << " "
		   << "commit " << SIMTOI_REVISION << ". "
		   << "See https://github.com/bkloppenborg/simtoi for information about "
		   << "this format";
	output["_file_info"] = header.str();

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
	this->setFixedSize(width, height);
}

void CGLWidget::SetTime(double time)
{
	mWorker->SetTime(time);
}

/// Set the wavelength to the specified value (in meters)
void CGLWidget::setWavelength(double wavelength)
{
	mWorker->SetWavelength(wavelength);
}

void CGLWidget::startRendering()
{
	// Shut off auto buffer swapping and call doneCurrent to release the OpenGL context
	setAutoBufferSwap(false);
    this->doneCurrent();

	// Tell the thread to start.
    mWorker->start();
}

void CGLWidget::stopRendering()
{
	if(!mWorker)
		return;

    mWorker->stop();
    mWorker->wait();

    this->makeCurrent();
}

//void CGLWidget::updateData(void)
//{
//	emit dataUpdated();
//}

void CGLWidget::updateParameters()
{
	mWorker->Render();
}
