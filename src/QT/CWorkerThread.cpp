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

CWorkerThread::CWorkerThread(CGLWidget *glWidget, QString exe_folder)
	: QThread(), mGLWidget(glWidget)
{
	mRun = true;
}

CWorkerThread::~CWorkerThread()
{

}

void CWorkerThread::Enqueue(WorkerOperations op)
{
	// get exclusive access to the operation queue
	QMutexLocker lock(&mOpQueueMutex);

	// Push the operation onto the queue
	mOpQueue.push(op);

	// Release the OpQueue semaphore to let the worker run.
	mOpQueueSemaphore.release(1);
	return;
}

/// Get the next operation from the queue.  This is a blocking function.
WorkerOperations CWorkerThread::GetNextOperation(void)
{
	// Try to access the OpQueue semaphore.
	mOpQueueSemaphore.acquire();

	// get exclusive access to the operation queue
	QMutexLocker lock(&mOpQueueMutex);
	// get the top operation and pop it from the queue
	WorkerOperations tmp = mOpQueue.top();
	mOpQueue.pop();

	return tmp;
}

void CWorkerThread::GetResiduals(valarray<double> & residuals)
{
	// Lock the thread until this operation completes.
	QMutexLocker lock(&mMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = &residuals;
	Enqueue(GET_RESIDUALS);
	mSemaphore.acquire(1);
}

void CWorkerThread::GetUncertainties(valarray<double> & uncertainties)
{
	// Lock the thread until this operation completes.
	QMutexLocker lock(&mMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = &uncertainties;
	Enqueue(GET_UNCERTAINTIES);
	mSemaphore.acquire(1);
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
	double half_width = mImageWidth * mScale;
	glOrtho(-half_width, half_width, -half_width, half_width, -mAreaDepth, mAreaDepth);

	CCL_GLThread::CheckOpenGLError("Error occurred during GL Thread Initialization.");

	// Now have the workers initialize any OpenGL objects they need
	mWorkerList->InitGL();

	// ########
	// OpenCL initialization
	// ########

	// Lastly, have the workers initialize any OpenCL objects they need
	mWorkerList->InitCL();

	// Everything is initialized, lets run.
	WorkerOperations op;
	while(mRun)
	{
		op = GetNextOperation();
		switch(op)
		{
		case GET_RESIDUALS:
			break;

		case GET_UNCERTAINTIES:
			break;

		case RENDER:
			break;

		default:
		case STOP:
			mRun = false;
			break;

		}
	}

	emit finished();
}
