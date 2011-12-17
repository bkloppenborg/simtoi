/*
 * COpenGLThread.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 */

#include "COpenGLThread.h"

#include <cstdio>

#include "CGLWidget.h"
#include "CModelList.h"
#include "CGLShaderList.h"

using namespace std;

/// Instance of the class.
COpenGLThread COpenGLThread::TheInstance;

COpenGLThread::COpenGLThread()
{
	mRun = true;

}

COpenGLThread::~COpenGLThread()
{
	// Delete the shader list.
	delete this->shader_list;

   // Free OpenGL memory:
	if(mFBO_depth) glDeleteRenderbuffers(1, &mFBO_depth);
	if(mFBO_texture) glDeleteTextures(1, &mFBO_texture);
	if(mFBO) glDeleteFramebuffers(1, &mFBO);
}

/// Static function for checking OpenGL errors:
void COpenGLThread::CheckOpenGLError(string function_name)
{
    GLenum status = glGetError(); // Check that status of our generated frame buffer
    // If the frame buffer does not report back as complete
    if (status != 0)
    {
        string errstr =  (const char *) gluErrorString(status);
        printf("Encountered OpenGL Error %x %s\n %s", status, errstr.c_str(), function_name.c_str());
        exit(0); // Exit the application
    }
}

/// Copy the contents from the internal rendering framebuffer to GL_BACK
/// then swap GL_FRONT and GL_BACK
void COpenGLThread::BlitToScreen()
{

    // Bind back to the default buffer (just in case something didn't do it),
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit the application-defined render buffer to the on-screen render buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_BACK);
    glBlitFramebuffer(0, 0, mWindow_width, mWindow_height, 0, 0, mWindow_width, mWindow_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glFinish();
}

GLOperations COpenGLThread::GetNext()
{

}

/// Gets the named shader.
CShader * COpenGLThread::GetShader(eGLShaders shader)
{
	// TODO: implement this in a thread-safe fashion.
	// Really this is only called during initialization and should be ok not being thread-friendly.
	return shader_list->GetShader(shader);
};

void COpenGLThread::Init(int window_width, int window_height, double scale, string shader_source_dir)
{
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
    double half_width = mWindow_width * scale / 2;
    glOrtho(-half_width, half_width, -half_width, half_width, -half_width, half_width);

    // Init the off-screen frame buffer.
    InitFrameBuffer();
}

void COpenGLThread::InitFrameBuffer(void)
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

void COpenGLThread::InitFrameBufferDepthBuffer(void)
{

    glGenRenderbuffers(1, &mFBO_depth); // Generate one render buffer and store the ID in mFBO_depth
    glBindRenderbuffer(GL_RENDERBUFFER, mFBO_depth); // Bind the mFBO_depth render buffer

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindow_width, mWindow_height); // Set the render buffer storage to be a depth component, with a width and height of the window

    CheckOpenGLError("initFrameBufferDepthBuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer
}

void COpenGLThread::InitFrameBufferTexture(void)
{
    glGenTextures(1, &mFBO_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, mFBO_texture); // Bind the texture mFBOtexture

    // Create the texture in red channel only 8-bit (256 levels of gray) in GL_BYTE (CL_UNORM_INT8) format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mWindow_width, mWindow_height, 0, GL_RED, GL_BYTE, NULL);
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

/// Render the models.
/// Don't call this routine using a different thread unless you have control of the OpenGL context.
void COpenGLThread::RenderModels()
{
	if(mModels != NULL)
		mModels->Render(this);
}

void COpenGLThread::Stop()
{
	mRun = false;

	// Now add a shutdown command to the queue;
}

// Main thread function.
int COpenGLThread::Thread()
{
	// This is a consume-only thread.  It should NEVER enqueue any operations to the queue otherwise it will be deadlocked.
	COpenGLThread * control = COpenGLThread::GetInstance();
	GLOperations event;
	CGLWidget * gl_widget;

	// Instruct QT that this thread will handle all rendering operations:
	gl_widget->makeCurrent();

	// Now do all of the initalization here:

	bool run = true;

	while(control->Run())
	{
		// Block until something is on the queue:
		event = control->GetNext();

		switch(event)
		{
		case GLOp_Resize:
			// Does nothing.
			break;

		case GLOp_BlitToScreen:
			control->BlitToScreen();
			gl_widget->swapBuffers();
			break;

		case GLOp_RenderModels:
			control->RenderModels();
			break;

		default:
			// Do nothing.
			break;
		}

		// Let OpenGL finish processing commands before continuing.
		glFinish();

		// Now release the event

	}
}
