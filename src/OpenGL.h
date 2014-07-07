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

#include <string>
#include <iostream>
#include <stdexcept>

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

#define OPENGL_SUCCESS 0
#define OPENGL_FAILURE 1

/// Checks for the status of an OpenGL error and generates an error message is one is
/// detected.
static int checkGLError(GLenum input, GLenum reference, std::string message, bool critical_error = true)
{
	if(critical_error && input != reference)
	{
		// Look up the OpenGL error message
		std::string error_string = (const char *) gluErrorString(input);
		std::cout << "Error: "<< message << " Error code : " << input << std::endl;;
		std::cout << error_string << std::endl;

		return OPENGL_FAILURE;
	}

	return OPENGL_SUCCESS;
}

/// Macro which checks and marks the source of an OpenGL error and throws a runtime_error if
#define CHECK_OPENGL_ERROR(actual, msg) \
    if(checkGLError(actual, GL_NO_ERROR, msg)) \
    { \
        std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
        throw runtime_error("OpenGL error detected."); \
    }

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
