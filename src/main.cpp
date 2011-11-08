/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <string>
#include <iostream>
#include <cstdio>

// OpenGL Headers:
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "main.h"
#include "CModelList.h"
#include "CModelSphere.h"

using namespace std;

// Globals
int window_width = 128;
int window_height = 128;
CModelList * models;

// OpenGL Globals:
GLuint fbo, fbo_depth, fbo_texture;

// Prints out help describing the options on the command line
void print_help()
{
	string usage = "";

	cout << usage << "\n";
}

void keyboard(unsigned char key, int x, int y)
{
    if(key == 'q' || key == 'Q' || key == '\27')
        exit(0);
}

void reshape (int w, int h)
{
    // Do nothing, we don't want to alter the scale.
}

void display(void)
{
	models->Render(fbo, window_width, window_height);
}

void OpenGL_cleanup()
{
   // Free OpenGL memory:
	glDeleteRenderbuffers(1, &fbo);
	glDeleteFramebuffers(1, &fbo_depth);
	glDeleteFramebuffers(1, &fbo_texture);
}

void OpenGL_init(void)
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
    // TODO: Use the window scale to determine these values:
    glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

    // Init the off-screen frame buffer.
    initFrameBuffer();
}

void initFrameBuffer(void)
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

void initFrameBufferDepthBuffer(void)
{

    glGenRenderbuffers(1, &fbo_depth); // Generate one render buffer and store the ID in fbo_depth
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth); // Bind the fbo_depth render buffer

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height); // Set the render buffer storage to be a depth component, with a width and height of the window

    CheckOpenGLError("initFrameBufferDepthBuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer
}

void initFrameBufferTexture(void)
{
    glGenTextures(1, &fbo_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, fbo_texture); // Bind the texture fbo_texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
        printf("Encountered OpenGL Error in %s\n %x %s\n", function_name.c_str(), status, errstr.c_str());
        exit(0); // Exit the application
    }
}

// The main routine.
int main(int argc, char *argv[])
{
	// TODO: Read in user-specified parameter / model file.

	models = new CModelList();

	CModel * tmp = new CModelSphere();
	tmp->SetPositionType(XY);
	models->Append(tmp);

	double junk[5] = {3, 4, 5, 0, 0};
	models->SetParameters(junk, 5);

	// Continue on with the rest of the code.
	window_width = 512;
	window_height = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_LUMINANCE | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);

    // Now init OpenGL and register callbacks:
    OpenGL_init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    //glutIdleFunc(idle);
    // Start the main loop:
    glutMainLoop();

	// Clean up memory
    OpenGL_cleanup();
    delete models;

	return 0;
}


#endif /* MAIN_CPP_ */
