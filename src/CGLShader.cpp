/*
 * CGLShader.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */
 
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

#include <cstdio>
#include <cstdlib>
#include "CGLShader.h"
#include "ReadTextFile.h"
#include "CCL_GLThread.h"

CGLShader::CGLShader(CGLShaderList::ShaderTypes type, string shader_dir, string base_filename, string friendly_name, int n_parameters, vector<string> parameter_names, vector<float> starting_values, vector< pair<float, float> > minmax)
{
	mType = type;
	mShader_dir = shader_dir;
	mBase_name = base_filename;
	mFriendlyName = friendly_name;
	mNParams = n_parameters;
	mParam_names = parameter_names;
	mParam_locations = new GLuint[n_parameters];
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;
	mMinMax = new pair<float, float>[mNParams];
	mStartingValues = new float[mNParams];

	for(int i = 0; i < mNParams; i++)
	{
		mStartingValues[i] = starting_values[i];
		mMinMax[i].first = minmax[i].first;
		mMinMax[i].second = minmax[i].second;
	}
}

CGLShader::~CGLShader()
{
	// Release OpenGL memory
	if(mProgram) glDeleteProgram(mProgram);
	if(mShader_vertex) glDeleteShader(mShader_vertex);
	if(mShader_fragment) glDeleteShader(mShader_fragment);

	// Now release object memory:
	delete[] mParam_locations;
	delete[] mMinMax;
	delete[] mStartingValues;
}

/// Compiles an OpenGL shader, checking for errors.
void CGLShader::CompileShader(GLuint shader)
{
    GLint tmp = GL_TRUE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &tmp);
    if(tmp != GL_TRUE)
    {
		char * infolog = (char*) malloc(501);
		int length;
		printf("Could not build shader!\n");
		glGetShaderInfoLog(shader, 500, &length, infolog);
		printf("%s\n", infolog);
    }
}

/// Returns the minimum parameter value, -1 if i is out of range.
float CGLShader::GetMin(unsigned int i)
{
	if(i < mNParams)
		return mMinMax[i].first;

	return -1;
}

/// Returns the maximum parameter value, -1 if i is out of range.
float CGLShader::GetMax(unsigned int i)
{
	if(i < mNParams)
		return mMinMax[i].second;

	return -1;
}

/// Returns the name of the specified parameter, if out of bounds returns an empty string.
string CGLShader::GetParamName(unsigned int i)
{
	if(i < mNParams)
		return mParam_names[i];

	return "";
}

float CGLShader::GetStartingValue(unsigned int i)
{
	if(i < mNParams)
		return mStartingValues[i];

	return 0;
}

// Loads the shader from the source file and creates a binary for the current selected context.
void CGLShader::Init()
{
	// If the shader is loaded, immediately exit the function
	if(mShaderLoaded)
		return;

    GLint tmp = GL_TRUE;

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
    if(!bool(glIsShader(mShader_vertex)))
    	CCL_GLThread::CheckOpenGLError("Could not create vertex shader.");

    mShader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if(!bool(glIsShader(mShader_vertex)))
    	CCL_GLThread::CheckOpenGLError("Could not create fragement shader shader.");

    glAttachShader(mProgram, mShader_vertex);
    glAttachShader(mProgram, mShader_fragment);

    // Now put the shader code into the object, compile and link.
    glShaderSource(mShader_vertex, 1, &tmp_source_v, NULL);
    glShaderSource(mShader_fragment, 1, &tmp_source_f, NULL);

    CompileShader(mShader_vertex);
    CompileShader(mShader_fragment);

    LinkProgram(mProgram);

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

/// Links an OpenGL program.
void CGLShader::LinkProgram(GLuint program)
{
	GLint tmp = GL_TRUE;
    glLinkProgram(mProgram);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &tmp);
    if(tmp == GL_FALSE)
    {
    	char * infolog = (char*) malloc(501);
    	int length;
    	printf("Could not build mProgram!");
    	glGetProgramInfoLog(mProgram, 500, &length, infolog);
    	printf("%s\n", infolog);
    }
}

void CGLShader::UseShader(double * params, unsigned int in_params)
{
	if(!mShaderLoaded)
		Init();

	//if(imNParams != this->mNParams)
	// throw exception

	// Tell OpenGL to use the mProgram
	glUseProgram(mProgram);

	// Set the parameters
	// NOTE: we intentionally downcast from double to GLfloat, loss of precision unavoidable here.
	GLfloat tmp;
	for(int i = 0; (i < mNParams && i < in_params); i++)
	{
		tmp = GLfloat(params[i]);
		glUniform1fv(mParam_locations[i], 1, &tmp);
	}
}
