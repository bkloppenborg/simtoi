
#include <QTime>
#include <QtDebug>

#include "CCL_GLThread.h"
#include "CGLWidget.h"
#include "CModel.h"
#include "CModelList.h"
#include "CGLShaderList.h"
#include "CPosition.h"
#include "CLibOI.h"

int CCL_GLThread::count = 0;

CCL_GLThread::CCL_GLThread(CGLWidget *glWidget, string shader_source_dir, string kernel_source_dir)
	: QThread(), mGLWidget(glWidget)
{
	id = count++;

    mRun = true;
    mPermitResize = true;
    mWidth = 1;
    mHeight = 1;
    mScale = 0.01;	// init to some value > 0.

    mModelList = new CModelList();
    mShaderList = new CGLShaderList(shader_source_dir);
    mDepth = 100; // +mDepth to -mDepth is the viewing region, in coordinate system units.

    mKernelSourceDir = kernel_source_dir;
    mCL = NULL;
    mCLInitalized = false;
    // These values come from outside of this class, if they are an array don't delete or free them here.
    mCLDataSet = 0;
    mCLValue = 0;
    mCLArrayValue = NULL;
    mCLArrayN = 0;
}

CCL_GLThread::~CCL_GLThread()
{
	delete mCL;
	delete mModelList;
	delete mShaderList;
}

/// Appends a model to the model list, importing shaders and features as necessary.
void CCL_GLThread::AddModel(CModelList::ModelTypes model)
{
	// Create the model, load the shader.
	CModel * tmp_model = mModelList->AddNewModel(model);

	// Initialize with default (XY) position and no shader.
	tmp_model->SetPositionType(CPosition::XY);
	CGLShaderWrapper * tmp_shader = mShaderList->GetShader(CGLShaderList::NONE);
	tmp_model->SetShader(tmp_shader);

	EnqueueOperation(GLT_RenderModels);
}

/// Copies the off-screen framebuffer to the on-screen buffer.  To be called only by the thread.
void CCL_GLThread::BlitToScreen()
{
    // Bind back to the default buffer (just in case something didn't do it),
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit the application-defined render buffer to the on-screen render buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
	/// TODO: In QT I don't know what GL_BACK is.  Seems GL_DRAW_FRAMEBUFFER is already set to it though.
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_BACK);
    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glFinish();
    mGLWidget->swapBuffers();
	CCL_GLThread::CheckOpenGLError("CGLThread::BlitToScreen()");
}

void CCL_GLThread::ClearQueue()
{
	// Clear the queue and reset the semaphore.
	mQueueMutex.lock();

	// Clear the queue
	while (!mQueue.empty())
	{
		mQueue.pop();
	}

	mQueueSemaphore.acquire(mQueueSemaphore.available());
	mQueueMutex.unlock();
}

/// Static function for checking OpenGL errors:
void CCL_GLThread::CheckOpenGLError(string function_name)
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

/// Enqueue an operation for the CCL_GLThread to process.
void CCL_GLThread::EnqueueOperation(CL_GLT_Operations op)
{
	// Lock the queue, append the item, increment the semaphore.
	mQueueMutex.lock();
	mQueue.push(op);
	mQueueMutex.unlock();
	mQueueSemaphore.release();
}

/// Returns the chi values in output for the specified data set and the current image.
void CCL_GLThread::GetChi(int data_num, float * output, int & n)
{
	mCLDataSet = data_num;
	mCLArrayValue = output;
	mCLArrayN = n;
	EnqueueOperation(CLT_Chi);
	mCLOpSemaphore.acquire();
	n = mCLArrayN;
}

/// Returns the chi2 for the specified data set
float CCL_GLThread::GetChi2(int data_num)
{
	// Set the data number, enqueue the operation and then block until we receive an answer.
	mCLDataSet = data_num;
	EnqueueOperation(CLT_Chi2);
	mCLOpSemaphore.acquire();
	return mCLValue;
}

/// Returns the average
double CCL_GLThread::GetDataAveTime(int data_num)
{
	return mCL->GetDataAveTime(data_num);
}

/// Returns the chi2 for the specified data set
float CCL_GLThread::GetLogLike(int data_num)
{
	// Set the data number, enqueue the operation and then block until we receive an answer.
	mCLDataSet = data_num;
	EnqueueOperation(CLT_LogLike);
	mCLOpSemaphore.acquire();
	return mCLValue;
}

/// Returns the total number of data points (V2 + T3) in all data sets loaded.
int CCL_GLThread::GetNData()
{
	return mCL->GetNData();
}

/// Returns the total allocation size for all data points (V2 + 2 * T3)
int CCL_GLThread::GetNDataAllocated()
{
	return mCL->GetNDataAllocated();
}

/// Returns the data allocation size for the data_num entry, returns zero if data_num is invalid.
int CCL_GLThread::GetNDataAllocated(int data_num)
{
	return mCL->GetNDataAllocated(data_num);
}

/// Returns the total number of data sets.
int CCL_GLThread::GetNDataSets()
{
	return mCL->GetNDataSets();
}

/// Get the next operation from the queue.  This is a blocking function.
CL_GLT_Operations CCL_GLThread::GetNextOperation(void)
{
	// First try to get access to the semaphore.  This is a blocking call if the queue is empty.
	mQueueSemaphore.acquire();
	// Now lock the queue, pull off the top item, pop it from the queue, and return.
	mQueueMutex.lock();
	CL_GLT_Operations tmp = mQueue.top();
	mQueue.pop();
	mQueueMutex.unlock();
	return tmp;
}

/// Returns a list of pairs of <eGlShader, string> corresponding to the (enumerated_name, friendly_name)
/// of the shaders stored in this object.
vector< pair<CGLShaderList::ShaderTypes, string> > CCL_GLThread::GetShaderNames(void)
{
	return mShaderList->GetTypes();
}

void CCL_GLThread::InitFrameBuffer(void)
{
    InitFrameBufferDepthBuffer();
    InitFrameBufferTexture();

    glGenFramebuffers(1, &mFBO); // Generate one frame buffer and store the ID in mFBO
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO); // Bind our frame buffer

    // Attach the depth and texture buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBO_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBO_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check that status of our generated frame buffer
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        string errorstring = (char *) gluErrorString(status);
        printf("Couldn't create frame buffer: %x %s\n", status, errorstring.c_str());
        exit(0); // Exit the application
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer
}

void CCL_GLThread::InitFrameBufferDepthBuffer(void)
{
    glGenRenderbuffers(1, &mFBO_depth); // Generate one render buffer and store the ID in mFBO_depth
    glBindRenderbuffer(GL_RENDERBUFFER, mFBO_depth); // Bind the mFBO_depth render buffer

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight); // Set the render buffer storage to be a depth component, with a width and height of the window

    CheckOpenGLError("initFrameBufferDepthBuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer
}

void CCL_GLThread::InitFrameBufferTexture(void)
{
    glGenTextures(1, &mFBO_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, mFBO_texture); // Bind the texture mFBOtexture

    // Create the texture in red channel only 8-bit (256 levels of gray) in GL_BYTE (CL_UNORM_INT8) format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mWidth, mHeight, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWindow_width, mWindow_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    // These other formats might work, check that GL_BYTE is still correct for the higher precision.
    // I don't think we'll ever need floating point numbers, but those are here too:
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, mWindow_width, mWindow_height, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32, mWindow_width, mWindow_height, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mWindow_width, mWindow_height, 0, GL_RED, CL_HALF_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWindow_width, mWindow_height, 0, GL_RED, GL_FLOAT, NULL);


    // Setup the basic texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

/// Loads data.
void CCL_GLThread::LoadData(string filename)
{
	if(mCL)
		mCL->LoadData(filename);
}

/// Resets any OpenGL errors by looping.
void CCL_GLThread::ResetGLError()
{
    while (glGetError() != GL_NO_ERROR) {};
}

/// Resize the window.  Normally called from QT
void CCL_GLThread::resizeViewport(const QSize &size)
{
    resizeViewport(size.width(), size.height());
}

/// Resize the window.  Called from external applications.
void CCL_GLThread::resizeViewport(int width, int height)
{
	if(mPermitResize)
	{
		mWidth = width;
		mHeight = height;
		EnqueueOperation(GLT_Resize);
	}
}   

/// Run the thread.
void CCL_GLThread::run()
{
#ifdef DEBUG
    printf("Starting Render Thread with ID %i\n", id);
#endif // DEBUG

    mGLWidget->makeCurrent();
	// ########
	// OpenGL initialization
	// ########
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// Set to flat (non-interpolated) shading:
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);    // enable the Z-buffer depth testing
	glDisable(GL_DITHER);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

	// Now setup the projection system to be orthographic
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Note, the coordinates here are in object-space, not coordinate space.
	double half_width = mWidth * mScale;// / 2;
	glOrtho(-half_width, half_width, -half_width, half_width, -mDepth, mDepth);

    // Init the off-screen frame buffer.
    InitFrameBuffer();

    // Start the thread
    mRun = true;
    EnqueueOperation(GLT_RenderModels);
    //EnqueueOperation(GLT_BlitToScreen);
    CL_GLT_Operations op;

	CCL_GLThread::CheckOpenGLError("Error occured during GL Thread Initialization.");

	// Now setup the OpenCL device.
	mCL = new CLibOI(CL_DEVICE_TYPE_GPU);
	mCL->SetImage_GLTB(mFBO);
	mCL->SetKernelSourcePath(mKernelSourceDir);
//	mCL->SetRoutineType(ROUTINE_DFT, FT_DFT);

    // Main thread loop
    while (mRun)
    {
        op = GetNextOperation();

        // NOTE: Resize and Render cascade.
        switch(op)
        {
        case GLT_Animate:
         	mModelList->IncrementTime();
         	QThread::msleep(40);
         	EnqueueOperation(GLT_RenderModels);
         	EnqueueOperation(GLT_Animate);
         	break;

        case GLT_Resize:
        	// Resize the screen, then cascade to a render and a blit.
#ifdef DEBUG
    		printf("Resizing to %i %i\n.", mWidth, mHeight);
#endif //DEBUG
            glViewport(0, 0, mWidth, mHeight);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            half_width = mWidth * mScale / 2;
			glOrtho(-half_width, half_width, -half_width, half_width, -mDepth, mDepth);
            glMatrixMode(GL_MODELVIEW);
        	CCL_GLThread::CheckOpenGLError("CGLThread GLT_Resize");
        	// Now tell OpenCL about the image (depth = 1 because we have only one layer)
        	mCL->SetImageInfo(mWidth, mHeight, 1, float(mScale));
        	mPermitResize = false;

        default:
        case GLT_RenderModels:
            // Render the models, then cascade to a blit to screen.
        	CCL_GLThread::CheckOpenGLError("CGLThread GLT_RenderModels");
            mModelList->Render(mFBO, mWidth, mHeight);

     	case GLT_BlitToScreen:
			BlitToScreen();
        	CCL_GLThread::CheckOpenGLError("CGLThread GLT_BlitToScreen");
			break;

        case GLT_Stop:
#ifdef DEBUG
            qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " STOP";
#endif //DEBUG
            mRun = false;
            break;

        case GLT_StopAnimate:
        	ClearQueue();
        	EnqueueOperation(GLT_RenderModels);
        	break;

        case CLT_Init:
        	// Init all LibOI routines
        	mCL->Init();
        	mCLInitalized = true;
        	break;

        case CLT_Chi:
        	// Copy the image to the buffer, compute the chi values, and initiate a copy to the
        	// local value.
        	mCL->CopyImageToBuffer(0);
        	mCL->ImageToChi(mCLDataSet, mCLArrayValue, mCLArrayN);
        	mCLOpSemaphore.release(1);
        	break;

        case CLT_Chi2:
        	// Copy the image into the buffer, compute the chi2, set the value, release the operation semaphore.
        	// TODO: Note the spectral data will need something special here.
        	mCL->CopyImageToBuffer(0);
        	mCLValue = mCL->ImageToChi2(mCLDataSet);
        	mCLOpSemaphore.release(1);
        	break;

        case CLT_LogLike:
        	// Copy the image into the buffer, compute the chi2, set the value, release the operation semaphore.
        	// TODO: Note the spectral data will need something special here.
        	mCL->CopyImageToBuffer(0);
        	mCLValue = mCL->ImageToLogLike(mCLDataSet);
        	mCLOpSemaphore.release(1);
        	break;
        }
    }
}

/// Sets the scale for the model.
void CCL_GLThread::SetFreeParameters(float * params, int n_params)
{
	mModelList->SetFreeParameters(params, n_params);
	EnqueueOperation(GLT_RenderModels);
}

void CCL_GLThread::SetPositionType(int model_id, CPosition::PositionTypes pos_type)
{
	mModelList->SetPositionType(model_id, pos_type);
}

/// Sets the scale for the model.
void CCL_GLThread::SetScale(double scale)
{
	if(scale > 0)
		mScale = scale;
}

void CCL_GLThread::SetShader(int model_id, CGLShaderList::ShaderTypes shader)
{
	CGLShaderWrapper * tmp_shader = mShaderList->GetShader(shader);
	mModelList->SetShader(model_id, tmp_shader);
}

void CCL_GLThread::SetTime(double t)
{
	mModelList->SetTime(t);
}

void CCL_GLThread::SetTimestep(double dt)
{
	mModelList->SetTimestep(dt);
}

/// Stop the thread.
void CCL_GLThread::stop()
{
    qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " STOP";
    EnqueueOperation(GLT_Stop);
}
