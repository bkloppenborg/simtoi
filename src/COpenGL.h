/*
 * COpenGL.h
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
 *
 * A minimial wrapper class to initialize OpenGL and keep track of
 * open contexts and/or OpenGL allocated memory.
 */

#ifndef COPENGL_H_
#define COPENGL_H_

#include <GL/gl.h>
#include <GL/glu.h>

#include <string>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include "CGLShaderList.h"

class CShader;

using namespace std;

void CheckOpenGLError(string function_name);

class COpenGL
{
protected:
	// Class variables for (a single) off-screen frame buffer.
	GLuint fbo;
	GLuint fbo_texture;
	GLuint fbo_depth;
	int window_width;
	int window_height;
	double scale;
	boost::interprocess::interprocess_mutex mutex;

	CGLShaderList * shader_list;

public:
	COpenGL(int window_width, int window_height, double scale, string shader_source_dir);
	~COpenGL();

	void BlitToScreen();

	// Inline a few functions
	GLuint GetFramebuffer() { return fbo; };
	GLuint GetFramebufferTexture() { return fbo_texture; };
	CShader * GetShader(eGLShaders shader) { return shader_list->GetShader(shader); };

	int GetWindowWidth() { return window_width; };
	int GetWindowHeight() { return window_height; };

	void init(int argc, char *argv[]);

	void Lock() { mutex.lock(); };
	void Unlock() {mutex.unlock(); };

protected:
	void initFrameBuffer(void);

	void initFrameBufferDepthBuffer(void);

	void initFrameBufferTexture(void);

};

#endif /* COPENGL_H_ */
