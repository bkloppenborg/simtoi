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

	// Now setup the projection system to be orthographic
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Note, the coordinates here are in object-space, not coordinate space.
	double half_width = mImageWidth * mImageScale;
	glOrtho(-half_width, half_width, -half_width, half_width, -mImageDepth, mImageDepth);

	CCL_GLThread::CheckOpenGLError("Error occurred during GL Thread Initialization.");

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

		case EXPORT:
			// uses mTempString

			mWorkerSemaphore.release(1);
			break;

		case GET_RESIDUALS:
			// uses mTempArray
			mWorkerSemaphore.release(1);
			break;

		case GET_UNCERTAINTIES:
			// uses mTempArray
			mWorkerSemaphore.release(1);
			break;

		case RENDER:
			mModelList->Render(0, mImageWidth, mImageHeight);
			break;

		case ANIMATE_STOP:
			ClearQueue();
			break;

		default:
		case STOP:
			mRun = false;
			break;

		}
	}

	// Release the semaphore blocking from the STOP call
	mWorkerSemaphore.release(1);

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
	mWorkerSemaphore.acquire(1);
}
