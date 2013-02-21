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

#include "CGLWidget.h"
#include "CTaskList.h"
#include "CModelList.h"

// X11 "Status" definition causes namespace issues. Include after any QT headers (https://bugreports.qt-project.org/browse/QTBUG-54)
#include "COpenCL.hpp"

extern string EXE_FOLDER;

CWorkerThread::CWorkerThread(CGLWidget *glWidget, QString exe_folder)
	: QThread(), mGLWidget(glWidget)
{
	mRun = true;

    // Init datamembers to something reasonable.
    mImageWidth = 128;
    mImageHeight = 128;
    mImageDepth = 1;
    mImageScale = 0.05;
    mImageSamples = 4;

	// Initialize the model and task lists:
    mModelList = CModelListPtr(new CModelList());
	mTaskList = CTaskListPtr(new CTaskList(this));
}

CWorkerThread::~CWorkerThread()
{
	// Stop the thread if it is running. This is a blocking call.
	if(mRun)
		stop();

	// Free local OpenGL objects.
	glDeleteFramebuffers(1, &mFBO);
	glDeleteFramebuffers(1, &mFBO_texture);
	glDeleteFramebuffers(1, &mFBO_depth);
	glDeleteFramebuffers(1, &mFBO_storage);
	glDeleteFramebuffers(1, &mFBO_storage_texture);
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

void CWorkerThread::BlitToBuffer(GLuint in_buffer, GLuint out_buffer)
{
	// TODO: Need to figure out how to use the layer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, in_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, out_buffer);
	//glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, out_buffer, 0, out_layer);
	glBlitFramebuffer(0, 0, mImageWidth, mImageHeight, 0, 0, mImageWidth, mImageHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glFinish();

  	CWorkerThread::CheckOpenGLError("CGLThread BlitToBuffer");
}

/// Blits the off-screen framebuffer to the foreground buffer.
void CWorkerThread::BlitToScreen(GLuint FBO)
{
    // Bind back to the default buffer (just in case something didn't do it),
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit the application-defined render buffer to the on-screen render buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

    /// TODO: In QT I don't know what GL_BACK is.  Seems GL_DRAW_FRAMEBUFFER is already set to it though.
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_BACK);
    glBlitFramebuffer(0, 0, mImageWidth, mImageHeight, 0, 0, mImageWidth, mImageHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    SwapBuffers();
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

void CWorkerThread::CreateGLBuffer(GLuint & FBO, GLuint & FBO_texture, GLuint & FBO_depth, GLuint & FBO_storage, GLuint & FBO_storage_texture)
{
	CreateGLMultisampleRenderBuffer(mImageWidth, mImageHeight, mImageSamples, FBO, FBO_texture, FBO_depth);
	CreateGLStorageBuffer(mImageWidth, mImageHeight, FBO_storage, FBO_storage_texture);
}

void CWorkerThread::CreateGLMultisampleRenderBuffer(unsigned int width, unsigned int height, unsigned int samples,
		GLuint & FBO, GLuint & FBO_texture, GLuint & FBO_depth)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenRenderbuffers(1, &FBO_texture);
	glBindRenderbuffer(GL_RENDERBUFFER, FBO_texture);
	// Create a 2D multisample texture
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA32F, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO_texture);

	glGenRenderbuffers(1, &FBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, FBO_depth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check that status of our generated frame buffer
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
    	const GLubyte * errStr = gluErrorString(status);
        printf("Couldn't create multisample frame buffer: %x %s\n", status, (char*)errStr);
        delete errStr;
        exit(0); // Exit the application
    }

    // All done, bind back to the default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CWorkerThread::CreateGLStorageBuffer(unsigned int width, unsigned int height, GLuint & FBO_storage, GLuint & FBO_storage_texture)
{
    glGenTextures(1, &FBO_storage_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, FBO_storage_texture); // Bind the texture mFBOtexture

    // Create the texture in red channel only 8-bit (256 levels of gray) in GL_BYTE (CL_UNORM_INT8) format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    // Enable this one for alpha blending:
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    // These other formats might work, check that GL_BYTE is still correct for the higher precision.
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, mWidth, mHeight, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32, mWidth, mHeight, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mWidth, mHeight, 0, GL_RED, CL_HALF_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWidth, mHeight, 0, GL_RED, GL_FLOAT, NULL);

    // Setup the basic texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &FBO_storage); // Generate one frame buffer and store the ID in mFBO
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_storage); // Bind our frame buffer

    // Attach the depth and texture buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO_storage_texture, 0);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBO_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check that status of our generated frame buffer
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
    	const GLubyte * errStr = gluErrorString(status);
        printf("Couldn't create storage frame buffer: %x %s\n", status, (char*)errStr);
        delete errStr;
        exit(0); // Exit the application
    }

    // All done, bind back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	mWorkerSemaphore.release(1);
}

unsigned int CWorkerThread::GetDataSize()
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// NOTE: this is a cross-thread call.
	return mTaskList->GetDataSize();
}

/// Gets a list of file filters for use in a QFileDialog
///
/// NOTE: This read-only operation is a cross-thread call
QStringList CWorkerThread::GetFileFilters()
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	vector<string> filters = mTaskList->GetFileFilters();
	QStringList output;

	for(auto filter: filters)
		output.append(QString::fromStdString(filter));

	return output;
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

void CWorkerThread::GetResiduals(double * residuals, unsigned int size)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = residuals;
	mTempArraySize = size;
	Enqueue(GET_RESIDUALS);

	// Wait for the operation to complete
	mWorkerSemaphore.acquire(1);
}

void CWorkerThread::GetUncertainties(double * uncertainties, unsigned int size)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Assign temporary storage, enqueue the command, and wait for completion
	mTempArray = uncertainties;
	mTempArraySize = size;
	Enqueue(GET_UNCERTAINTIES);

	// Wait for the operation to complete
	mWorkerSemaphore.acquire(1);
}

void CWorkerThread::OpenData(string filename)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	mTempString = filename;
	Enqueue(OPEN_DATA);

	// Wait for the operation to complete.
	mWorkerSemaphore.acquire(1);
}

/// Instructs the thread to render to the default framebuffer.
void CWorkerThread::Render()
{
	// Exclusive access is not required for a rendering operation.
	// Simply enqueue and return.
	Enqueue(RENDER);
}

void CWorkerThread::Restore(Json::Value input)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	// Note, this is a cross-thread call.
	mModelList->Restore(input);
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

	// Resize the screen, then cascade to a render and a blit.
    glViewport(0, 0, mImageWidth, mImageHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double half_width = mImageWidth * mImageScale / 2;
    double depth = 500; // hard-coded to 500 units (typically mas) in each direction.
	glOrtho(-half_width, half_width, -half_width, half_width, -depth, depth);
    glMatrixMode(GL_MODELVIEW);

	CWorkerThread::CheckOpenGLError("Error occurred during GL Thread Initialization.");

	// Now create this thread's off-screen buffers to match other off-screen buffers
	// All rendering of objects from the UI happens here.
	CreateGLBuffer(mFBO, mFBO_texture, mFBO_depth, mFBO_storage, mFBO_storage_texture);

	// Now have the workers initialize any OpenGL objects they need
	mTaskList->InitGL();

	// ########
	// OpenCL initialization (must occur after OpenGL init)
	// ########
	mOpenCL = COpenCLPtr(new COpenCL(CL_DEVICE_TYPE_GPU));

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

		case EXPORT:
			// Instruct the worker to export data
			mTaskList->Export(mTempString);
			mWorkerSemaphore.release(1);
			break;

		case GET_RESIDUALS:
			// uses mTempArray
			mTaskList->GetResiduals(mTempArray, mTempArraySize);
			mWorkerSemaphore.release(1);
			break;

		case GET_UNCERTAINTIES:
			// uses mTempArray
			mTaskList->GetUncertainties(mTempArray, mTempArraySize);
			mWorkerSemaphore.release(1);
			break;

		case OPEN_DATA:
			// Instruct the task list to open the file.
			mTaskList->OpenData(mTempString);
			mWorkerSemaphore.release(1);

		case RENDER:
			mModelList->Render(mFBO, mImageWidth, mImageHeight);
			BlitToScreen(mFBO);
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

void CWorkerThread::SetScale(double scale)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	if(scale < 0)
		throw runtime_error("Image scale cannot be negative.");

	mImageScale = scale;
}

void CWorkerThread::SetSize(unsigned int width, unsigned int height)
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	if(width < 1 || height < 1)
		throw runtime_error("Image size must be at least 1x1 pixel");

	mImageWidth = width;
	mImageHeight = height;
}

Json::Value CWorkerThread::Serialize()
{
	// Get exclusive access to the worker
	QMutexLocker lock(&mWorkerMutex);

	Json::Value temp = mModelList->Serialize();

	return temp;
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

void CWorkerThread::SwapBuffers()
{
	glFinish();
    mGLWidget->swapBuffers();
    CWorkerThread::CheckOpenGLError("CWorkerThread::SwapBuffers()");
}
