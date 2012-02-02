/*
 * CGLShader.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include <cstdio>
#include <cstdlib>
#include "CGLShader.h"
#include "ReadTextFile.h"
#include "CCL_GLThread.h"

CGLShader::CGLShader(eGLShaders type, string shader_dir, string base_filename, string friendly_name, int n_parameters, vector<string> parameter_names)
{
	mType = type;
	mShader_dir = shader_dir;
	mBase_name = base_filename;
	mFriendlyName = friendly_name;
	mNParams = n_parameters;
	mParam_names = parameter_names;
	mParam_locations = new GLuint[n_parameters];
	mShaderLoaded = false;
	mProgram = NULL;
	mShader_vertex = NULL;
	mShader_fragment = NULL;
}

CGLShader::~CGLShader()
{
	// Release OpenGL memory
	if(mProgram) glDeleteProgram(mProgram);
	if(mShader_vertex) glDeleteShader(mShader_vertex);
	if(mShader_fragment) glDeleteShader(mShader_fragment);

	// Now release object memory:
	delete[] mParam_locations;
}

/// Returns the name of the specified parameter, if out of bounds returns an empty string.
string CGLShader::GetParamName(int i)
{
	if(i < mNParams)
		return mParam_names[i];

	return "";
}

// Loads the shader from the source file and creates a binary for the current selected context.
void CGLShader::Init()
{
	// If the shader is loaded, immediately exit the function
	if(mShaderLoaded)
		return;

    // First load the mProgram source.
    string source_v, source_f;
	const GLchar * tmp_source_v;
	const GLchar * tmp_source_f;
    source_v = ReadFile(mShader_dir + '/' + mBase_name + ".vert", "Could not read " + mShader_dir + '/' + mBase_name + ".vert file!");
    source_f = ReadFile(mShader_dir + '/' + mBase_name + ".frag", "Could not read " + mShader_dir + '/' + mBase_name + ".frag file!");
	tmp_source_v = (const GLchar *) source_v.c_str();
	tmp_source_f = (const GLchar *) source_f.c_str();

    // Now create the mProgram with vertex and fragement shaders
	mProgram = glCreateProgram();
    CCL_GLThread::CheckOpenGLError("Could not create shader mProgram.");
    mShader_vertex = glCreateShader(GL_VERTEX_SHADER);
    mShader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(mProgram, mShader_vertex);
    glAttachShader(mProgram, mShader_fragment);

    // Now put the shader code into the object, compile and link.
    glShaderSource(mShader_vertex, 1, &tmp_source_v, NULL);
    glShaderSource(mShader_fragment, 1, &tmp_source_f, NULL);

    glCompileShader(mShader_vertex);
    glCompileShader(mShader_fragment);
    glLinkProgram(mProgram);

    GLint tmp;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &tmp);
    if(tmp == GL_FALSE)
    {
    	char * infolog = (char*) malloc(501);
    	int length;
    	printf("Could not build mProgram!");
    	glGetProgramInfoLog(mProgram, 500, &length, infolog);
    	printf("%s\n", infolog);
    }

    CCL_GLThread::CheckOpenGLError("Could not link shader mProgram.");

    // Now look up the locations of the parameters
    for(int i = 0; i < mNParams; i++)
    {
    	mParam_locations[i] = glGetUniformLocation(mProgram, mParam_names[i].c_str());
    	CCL_GLThread::CheckOpenGLError("Could find variable in shader source.");
    }

    // The shader has been loaded, compiled, and linked.
    mShaderLoaded = true;
}

void CGLShader::UseShader(float * params, int imNParams)
{
	if(!mShaderLoaded)
		Init();

	//if(imNParams != this->mNParams)
	// throw exception

	// Tell OpenGL to use the mProgram
	glUseProgram(mProgram);

	// Set the parameters.
	for(int i = 0; i < mNParams; i++)
		glUniform1fv(mParam_locations[i], 1, &params[i]);
}
