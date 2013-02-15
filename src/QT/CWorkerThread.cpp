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

#include "CWorkerThread.h"
#include <QMutexLocker>
#include <GL/gl.h>
#include <GL/glu.h>

#include "CGLWidget.h"
#include "CTaskList.h"
#include "CModelList.h"

CWorkerThread::CWorkerThread(CGLWidget *glWidget, QString exe_folder)
	: QThread(), mGLWidget(glWidget)
{
	mRun = true;
	mTaskList = shared_ptr<CTaskList>(new CTaskList());
    mModelList = shared_ptr<CModelList>(new CModelList());

    // Init datamembers to something reasonable.
    mImageWidth = 128;
    mImageHeight = 128;
    mImageDepth = 1;
    mImageScale = 0.05;
}

CWorkerThread::~CWorkerThread()
{
	// Stop the thread if it is running. This is a blocking call.
	if(mRun)
		stop();
}

/// Appends a model to the list of models.
void CWorkerThread::AddModel(CModelPtr model)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Modify the list of models:
	mModelList->AddModel(model);
	Enqueue(RENDER);
}

/// Blits the off-screen framebuffer to the foreground buffer.
void CWorkerThread::BlitToScreen()
{
    mGLWidget->swapBuffers();
}

/// Static function for checking OpenGL errors:
void CWorkerThread::CheckOpenGLError(string function_name)
{
    GLenum status = glGetError(); // Check that status of our generated frame buffer
    // If the frame buffer does not report back as complete
    if (status != 0)
    {
        string errstr =  (const char *) gluErrorString(status);
        printf("Encountered OpenGL Error %x %s\n %s", status, errstr.c_str(), function_name.c_str());
        throw;
    }
}

// Clears the worker task queue.
void CWorkerThread::ClearQueue()
{
	// get exclusive access to the operation queue
	QMutexLocker lock(&mTaskMutex);

	// Clear the queue
	while (!mTaskQueue.empty())
		mTaskQueue.pop();

	mTaskSemaphore.acquire(mTaskSemaphore.available());
}

void CWorkerThread::Enqueue(WorkerOperations op)
{
	// get exclusive access to the operation queue
	QMutexLocker lock(&mTaskMutex);

	// Push the operation onto the queue
	mTaskQueue.push(op);

	// Release the OpQueue semaphore to let the worker run.
	mTaskSemaphore.release(1);
}

void CWorkerThread::ExportResults(QString save_folder)
{
	// get exclusive access to the operation queue
	QMutexLocker lock(&mTaskMutex);

	mTempString = save_folder.toStdString();
	Enqueue(EXPORT);
	mTaskSemaphore.release(1);
}

/// Get the next operation from the queue.  This is a blocking function.
WorkerOperations CWorkerThread::GetNextOperation(void)
{
	// Try to access the OpQueue semaphore.
	mTaskSemaphore.acquire();

	// get exclusive access to the operation queue
	QMutexLocker lock(&mTaskMutex);
	// get the top operation and pop it from the queue
	WorkerOperations tmp = mTaskQueue.top();
	mTaskQueue.pop();

	return tmp;
}

void CWorkerThread::GetResiduals(valarray<double> & residuals)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = &residuals;
	Enqueue(GET_RESIDUALS);
	mWorkerSemaphore.acquire(1);
}

void CWorkerThread::GetUncertainties(valarray<double> & uncertainties)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = &uncertainties;
	Enqueue(GET_UNCERTAINTIES);
	mWorkerSemaphore.acquire(1);
}

/// Instructs the thread to render to the default framebuffer.
void CWorkerThread::Render()
{
	// Exclusive access is not required for a rendering operation.
	// Simply enqueue and return.
	Enqueue(RENDER);
}

void CWorkerThread::Resize(unsigned int width, unsigned int height)
{
	// Resize the screen, then cascade to a render and a blit.
    glViewport(0, 0, mImageWidth, mImageHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double half_width = mImageWidth * mImageScale / 2;
	glOrtho(-half_width, half_width, -half_width, half_width, -mImageDepth, mImageDepth);
    glMatrixMode(GL_MODELVIEW);

    CWorkerThread::CheckOpenGLError("CWorkerThread Resize(int, int)");
	Enqueue(RENDER);
}

/// Resize the window.  Normally called from QT
void CWorkerThread::resizeViewport(const QSize &size)
{
    resizeViewport(size.width(), size.height());
}

/// Resize the window.  Called from external applications.
void CWorkerThread::resizeViewport(int width, int height)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	mImageWidth = width;
	mImageHeight = height;
	Enqueue(RESIZE);
}



// The main function of this thread
void CWorkerThread::run()
{
	// ########
	// OpenGL initialization
	// ########

    // claim the OpenGL context:
    mGLWidget->makeCurrent();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	// Set to flat (non-interpolated) shading:
	glShadeModel(GL_FLAT);
	glDisable(GL_DITHER);
	glEnable(GL_DEPTH_TEST);    // enable the Z-buffer depth testing

	// Enable alpha blending:
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable multisample anti-aliasing.
	glEnable(GL_MULTISAMPLE);

	Resize(mImageWidth, mImageHeight);

	CWorkerThread::CheckOpenGLError("Error occurred during GL Thread Initialization.");

	// Now have the workers initialize any OpenGL objects they need
	mTaskList->InitGL();

	// ########
	// OpenCL initialization
	// ########

	// Lastly, have the workers initialize any OpenCL objects they need
	mTaskList->InitCL();

	// Everything is initialized, lets run.
	WorkerOperations op;
	while(mRun)
	{
		op = GetNextOperation();
		switch(op)
		{
		case ANIMATE:
			mModelList->SetTime(mModelList->GetTime() + mTempDouble);
			Enqueue(RENDER);
			Enqueue(ANIMATE);
			break;

		case ANIMATE_STOP:
			ClearQueue();
			break;

		case BLIT_TO_SCREEN:
		    BlitToScreen();
			break;

		case EXPORT:
			mTaskList->Export(mTempString);
			mWorkerSemaphore.release(1);
			break;

		case GET_RESIDUALS:
			// uses mTempArray
			mTaskList->GetResiduals(*mTempArray);
			mWorkerSemaphore.release(1);
			break;

		case GET_UNCERTAINTIES:
			// uses mTempArray
			mTaskList->GetUncertainties(*mTempArray);
			mWorkerSemaphore.release(1);
			break;

		case RENDER:
			mModelList->Render(0, mImageWidth, mImageHeight);
			Enqueue(BLIT_TO_SCREEN);
			break;

		case RESIZE:
        	Resize(mImageWidth, mImageHeight);
        	break;

		default:
		case STOP:
			ClearQueue();
			mRun = false;
			break;

		}
	}

	emit finished();
}

void CWorkerThread::startAnimation(double timestep)
{
	QMutexLocker lock(&mWorkerMutex);
	mTempDouble = timestep;
	Enqueue(ANIMATE);
}

void CWorkerThread::stopAnimation()
{
	QMutexLocker lock(&mWorkerMutex);
	Enqueue(ANIMATE_STOP);
}

void CWorkerThread::stop()
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);
	// Equeue a stop command
	Enqueue(STOP);
}
