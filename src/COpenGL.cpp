/*
 * COpenGL.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
 */

#include <cstdio>
#include "COpenGL.h"
#include "CGLShaderList.h"

COpenGL::COpenGL(int window_width, int window_height, double scale, string shader_source_dir)
{
	// Init datamembers.
	fbo = fbo_depth = fbo_texture = 0;
	this->window_width = window_width;
	this->window_height = window_height;
	this->scale = scale;

	this->shader_list = new CGLShaderList(shader_source_dir);

}

COpenGL::~COpenGL()
{
	delete this->shader_list;

   // Free OpenGL memory:
	if(fbo_depth) glDeleteRenderbuffers(1, &fbo_depth);
	if(fbo_texture) glDeleteTextures(1, &fbo_texture);
	if(fbo) glDeleteFramebuffers(1, &fbo);
}

GLuint COpenGL::GetFramebuffer()
{
	return fbo;
}

GLuint COpenGL::GetFramebufferTexture()
{
	return fbo_texture;
}

CShader * COpenGL::GetShader(eGLShaders shader)
{
	return shader_list->GetShader(shader);
}

void COpenGL::init(int argc, char *argv[])
{
    // ########
    // Window Manager (GLUT) initialization
    // ########
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_LUMINANCE | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);

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
    double half_width = window_width * scale / 2;
    glOrtho(-half_width, half_width, -half_width, half_width, -half_width, half_width);

    // Init the off-screen frame buffer.
    initFrameBuffer();
}

void COpenGL::initFrameBuffer(void)
{
    initFrameBufferDepthBuffer();
    initFrameBufferTexture();

    glGenFramebuffers(1, &fbo); // Generate one frame buffer and store the ID in fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind our frame buffer

    // Attach the depth and texture buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);

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

void COpenGL::initFrameBufferDepthBuffer(void)
{

    glGenRenderbuffers(1, &fbo_depth); // Generate one render buffer and store the ID in fbo_depth
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth); // Bind the fbo_depth render buffer

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height); // Set the render buffer storage to be a depth component, with a width and height of the window

    CheckOpenGLError("initFrameBufferDepthBuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer
}

void COpenGL::initFrameBufferTexture(void)
{
    glGenTextures(1, &fbo_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, fbo_texture); // Bind the texture fbo_texture

    // Create the texture in red channel only 8-bit (256 levels of gray) in GL_BYTE (CL_UNORM_INT8) format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, window_width, window_height, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    // These other formats might work, check that GL_BYTE is still correct for the higher precision.
    // I don't think we'll ever need floating point numbers, but those are here too:
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, window_width, window_height, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32, window_width, window_height, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, window_width, window_height, 0, GL_RED, CL_HALF_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, window_width, window_height, 0, GL_RED, GL_FLOAT, NULL);


    // Setup the basic texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CheckOpenGLError(string function_name)
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
