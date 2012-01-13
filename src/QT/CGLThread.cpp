
#include <QTime>
#include <QtDebug>

#include "CGLThread.h"
#include "CGLWidget.h"
#include "CModel.h"
#include "CModelList.h"
#include "CGLShaderList.h"

int CGLThread::count = 0;

CGLThread::CGLThread(CGLWidget *glWidget, string shader_source_dir)
	: QThread(), mGLWidget(glWidget)
{
    mRun = true;
    mPermitResize = true;
    mResizeInProgress = false;
    mScale = 0.01;	// init to some value > 0.

    mModelList = new CModelList();
    mShaderList = new CGLShaderList(shader_source_dir);
}

CGLThread::~CGLThread()
{
	delete mModelList;
	delete mShaderList;
}

/// Appends a model to the model list, importing shaders and features as necessary.
void CGLThread::AddModel(eModels model, eGLShaders shader)
{
	// Create the model, load the shader.
	CModel * tmp_model = mModelList->AddNewModel(model);
	CGLShaderWrapper * tmp_shader = mShaderList->GetShader(shader);
	tmp_model->SetShader(tmp_shader);

	EnqueueOperation(GLT_RenderModels);

}

/// Static function for checking OpenGL errors:
void CGLThread::CheckOpenGLError(string function_name)
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

/// Copies the off-screen framebuffer to the on-screen buffer.  To be called only by the thread.
void CGLThread::BlitToScreen()
{
    // Bind back to the default buffer (just in case something didn't do it),
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit the application-defined render buffer to the on-screen render buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_BACK);
    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glFinish();
    mGLWidget->swapBuffers();
}

/// Enqueue an operation for the CGLThread to process.
void CGLThread::EnqueueOperation(GLT_Operations op)
{
	// Lock the queue, append the item, increment the semaphore.
	mQueueMutex.lock();
	mQueue.push(op);
	mQueueMutex.unlock();
	mQueueSemaphore.release();
}

/// Get the next operation from the queue.  This is a blocking function.
GLT_Operations CGLThread::GetNextOperation(void)
{
	// First try to get access to the semaphore.  This is a blocking call if the queue is empty.
	mQueueSemaphore.acquire();
	// Now lock the queue, pull off the top item, pop it from the queue, and return.
	mQueueMutex.lock();
	GLT_Operations tmp = mQueue.top();
	mQueue.pop();
	mQueueMutex.unlock();
	return tmp;
}

/// Returns a list of pairs of <eGlShader, string> corresponding to the (enumerated_name, friendly_name)
/// of the shaders stored in this object.
vector< pair<eGLShaders, string> > CGLThread::GetShaderNames(void)
{
	return mShaderList->GetShaderNames();
}

void CGLThread::InitFrameBuffer(void)
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

void CGLThread::InitFrameBufferDepthBuffer(void)
{
    glGenRenderbuffers(1, &mFBO_depth); // Generate one render buffer and store the ID in mFBO_depth
    glBindRenderbuffer(GL_RENDERBUFFER, mFBO_depth); // Bind the mFBO_depth render buffer

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight); // Set the render buffer storage to be a depth component, with a width and height of the window

    CheckOpenGLError("initFrameBufferDepthBuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer
}

void CGLThread::InitFrameBufferTexture(void)
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

void CGLThread::glDrawTriangle()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View

    glTranslatef(-1.5f,0.0f,-6.0f);

    glRotatef(rotAngle,0.0f,1.0f,0.0f);		// Rotate The Pyramid On The Y axis

    // draw a pyramid (in smooth coloring mode)
    glBegin(GL_POLYGON);				// start drawing a pyramid

    // front face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
    glVertex3f(0.0f, 1.0f, 0.0f);		        // Top of triangle (front)
    glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
    glVertex3f(-1.0f,-1.0f, 1.0f);		// left of triangle (front)
    glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
    glVertex3f(1.0f,-1.0f, 1.0f);		        // right of traingle (front)

    // right face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Right)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f( 1.0f,-1.0f, 1.0f);		// Left Of Triangle (Right)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);		// Right Of Triangle (Right)

    // back face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Back)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);		// Left Of Triangle (Back)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f(-1.0f,-1.0f, -1.0f);		// Right Of Triangle (Back)

    // left face of pyramid.
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Left)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f(-1.0f,-1.0f,-1.0f);		// Left Of Triangle (Left)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f(-1.0f,-1.0f, 1.0f);		// Right Of Triangle (Left)

    glEnd();					// Done Drawing The Pyramid
}

/// Resets any OpenGL errors by looping.
void CGLThread::ResetGLError()
{
    while (glGetError() != GL_NO_ERROR) {};
}

/// Resize the window.  Normally called from QT
void CGLThread::resizeViewport(const QSize &size)
{
    resizeViewport(size.width(), size.height());
}

/// Resize the window.  Called from external applications.
void CGLThread::resizeViewport(int width, int height)
{
	if(mPermitResize)
	{
		mWidth = width;
		mHeight = height;
		// For SIMTOI, only permit a resize once.
		EnqueueOperation(GLT_Resize);
	}
}   

/// Run the thread.
void CGLThread::run()
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
	double half_width = mWidth * mScale / 2;
	glOrtho(-half_width, half_width, -half_width, half_width, -half_width, half_width);

    // Init the off-screen frame buffer.
    InitFrameBuffer();

    // Start the thread
    mRun = true;
    EnqueueOperation(GLT_RenderModels);
    EnqueueOperation(GLT_BlitToScreen);
    GLT_Operations op;

    // Main thread loop
    // NOTE: If compiled with -D DEBUG_GL model rendering is skipped and a spinning pyramid is shown
    //       at the maximum possible framerate.
    while (mRun)
    {
        op = GetNextOperation();

        // NOTE: Resize and Render cascade.
        switch(op)
        {
        case GLT_Resize:
        	// Resize the screen.
#ifdef DEBUG
        	printf("Resizing...\n");
#endif //DEBUG
            glViewport(0, 0, mWidth, mHeight);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            half_width = mWidth * mScale / 2;
			glOrtho(-half_width, half_width, -half_width, half_width, -half_width, half_width);
            glMatrixMode(GL_MODELVIEW);

        case GLT_RenderModels:
        	// Call the drawing functions
#ifdef DEBUG_GL
        	// Bind to the off-screen framebuffer, clear it.
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers
            rotAngle = rotAngle + (id+1)*3; // threads rotate pyramid at different rate!
            qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << ":rendering...";
            glDrawTriangle();
            // Let the drawing operation complete.
            glFinish();
#else // DEBUG_GL

            mModelList->Render(mFBO, mWidth, mHeight);

#endif // DEBUG_GL
            // Let the rendering complete, then blit to the screen

     	case GLT_BlitToScreen:
			BlitToScreen();
			mGLWidget->updateGL();
			mGLWidget->swapBuffers();
#ifdef DEBUG_GL
			EnqueueOperation(GLT_Redraw);
#endif //DEBUG_GL
			break;

        case GLT_Stop:
#ifdef DEBUG
            qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " STOP";
#endif //DEBUG
            mRun = false;
            break;

        default:
            break;

        }
    }
}

/// Sets the scale for the model.
void CGLThread::SetScale(double scale)
{
	if(scale > 0)
		mScale = scale;
}

/// Stop the thread.
void CGLThread::stop()
{
    qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " STOP";
    EnqueueOperation(GLT_Stop);
}
