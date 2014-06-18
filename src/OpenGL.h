/*
 * OpenGL.h
 *
 *  Created on: Jun 18, 2014
 *      Author: bkloppenborg
 *
 *  Header file containing the standard OpenGL includes, plus workarounds for
 *  missing/oddly placed definitions on various OSes that are needed for SIMTOI.
 */

#ifndef OPENGL_H_
#define OPENGL_H_

// Standard OpenGL Headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// Workaround for GL_TEXTURE_RECTANGLE not being defined on CentOS 6.5
// GL_TEXTURE_RECTANGLE_ARB is defined though.
#ifndef GL_TEXTURE_RECTANGLE
	#ifdef GL_TEXTURE_RECTANGLE_ARB
		#define GL_TEXTURE_RECTANGLE GL_TEXTURE_RECTANGLE_ARB
	#else
		#error "Neither GL_TEXTURE_RECTANGLE nor GL_TEXTURE_RECTANGLE_ARB are defined."
	#endif
#endif // GL_TEXTURE_RECTANGLE


#endif /* OPENGL_H_ */
