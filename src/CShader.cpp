/*
 * CShader.cpp
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
#include <stdexcept>
#include <sstream>
#include "CShader.h"
#include "textio.hpp"
#include "CWorkerThread.h"
#include "json/json.h"

using namespace std;

CShader::CShader(const CShader & other)
{
	id = other.id;
	name = other.name;
	mShader_dir = other.mShader_dir;
	mVertShaderFileName = other.mVertShaderFileName;
	mFragShaderFilename = other.mFragShaderFilename;
	mParams = other.mParams;
	mParam_locations.resize(mParams.size());
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;
}

// Constructor from JSON save file
CShader::CShader(string json_config_file)
{
	// Read in the JSON configuration file:
	Json::Reader reader;
	Json::Value input;
	string file_contents = ReadFile(json_config_file, "Could not read shader save file: '" + json_config_file + "'.");
	bool parsingSuccessful = reader.parse(file_contents, input);
	if(!parsingSuccessful)
	{
		string error_message = reader.getFormatedErrorMessages();

		throw runtime_error("Could not parse shader section of the configuration file: \n'"
				+ json_config_file + "'\n"
				+ "The error message follows: \n"
				+ error_message);
	}

	// Start to initalize this object. First get the ID and name
	id = input["shader_id"].asString();
	name = input["shader_name"].asString();

	// Shaders and configuration files must be in the same directory.
	size_t folder_end = json_config_file.find_last_of("/\\");
	mShader_dir = json_config_file.substr(0,folder_end+1);
	mVertShaderFileName = input["vertex_shader"].asString();
	mFragShaderFilename = input["fragment_shader"].asString();

	// Now read in the parameters from the file
	stringstream tmp;
	string param_label;
	for(int i = 0; true; i++)
	{
		// reset the stringstream
		tmp.str("");
		tmp.clear();
		// create the parameter lookup.
		tmp << "param_" << i;
		param_label = tmp.str();
		if(!input.isMember(param_label))
			break;

		string param_id = input[param_label]["id"].asString();
		string param_name = input[param_label]["name"].asString();
		double param_min = input[param_label]["min"].asDouble();
		double param_max = input[param_label]["max"].asDouble();
		double param_val = input[param_label]["value"].asDouble();
		string param_help = input[param_label]["help"].asString();

		// init the step size to something reasonable, say 1/10th of the range.
		double step_size = (param_max - param_min) / 10;

		cout << "Read in ID: " << param_id << " and name: " << param_name << endl;

		// Add the parameter to this object.
		addParameter(param_id, param_val, param_min, param_max, false, step_size, param_name, param_help);
	}

	//
	// Init remaining CShader members
	//
	mParam_locations.resize(mParams.size());
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;
}

CShader::~CShader()
{
	// Release OpenGL memory
	if(mProgram) glDeleteProgram(mProgram);
	if(mShader_vertex) glDeleteShader(mShader_vertex);
	if(mShader_fragment) glDeleteShader(mShader_fragment);
}

/// Compiles an OpenGL shader, checking for errors.
void CShader::CompileShader(GLuint shader)
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

/// Returns a reference to the shader program, loading it into memory if necessary.
GLuint CShader::GetProgram()
{
	if(!mShaderLoaded)
		Init();

	return mProgram;
}

// Loads the shader from the source file and creates a binary for the current selected context.
void CShader::Init()
{
	// If the shader is loaded, immediately exit the function
	if(mShaderLoaded)
		return;

    GLint tmp = GL_TRUE;

    // First load the mProgram source.
    string source_v, source_f;
	const GLchar * tmp_source_v;
	const GLchar * tmp_source_f;
    source_v = ReadFile(mShader_dir + '/' + mVertShaderFileName,
    		"Could not read " + mShader_dir + '/' + mVertShaderFileName + " file!");
    source_f = ReadFile(mShader_dir + '/' + mFragShaderFilename,
    		"Could not read " + mShader_dir + '/' + mFragShaderFilename + " file!");
	tmp_source_v = (const GLchar *) source_v.c_str();
	tmp_source_f = (const GLchar *) source_f.c_str();

    // Now create the mProgram with vertex and fragement shaders
	mProgram = glCreateProgram();
    CWorkerThread::CheckOpenGLError("Could not create shader mProgram.");
    mShader_vertex = glCreateShader(GL_VERTEX_SHADER);
    if(!bool(glIsShader(mShader_vertex)))
    	CWorkerThread::CheckOpenGLError("Could not create vertex shader.");

    mShader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if(!bool(glIsShader(mShader_vertex)))
    	CWorkerThread::CheckOpenGLError("Could not create fragment shader.");

    glAttachShader(mProgram, mShader_vertex);
    glAttachShader(mProgram, mShader_fragment);

    // Now put the shader code into the object, compile and link.
    glShaderSource(mShader_vertex, 1, &tmp_source_v, NULL);
    glShaderSource(mShader_fragment, 1, &tmp_source_f, NULL);

    CompileShader(mShader_vertex);
    CompileShader(mShader_fragment);

    LinkProgram(mProgram);

    CWorkerThread::CheckOpenGLError("Could not link shader mProgram.");

    // Now the shader-specific parameters:
    unsigned int i = 0;
    for(auto it: mParams)
    {
    	mParam_locations[i] = glGetUniformLocation(mProgram, it.second.getID().c_str());
    	cout << "ID: " << it.second.getID() << " Location: " << mParam_locations[i] << endl;
    	CWorkerThread::CheckOpenGLError("Could find variable in shader source.");
    	i++;
    }

    // The shader has been loaded, compiled, and linked.
    mShaderLoaded = true;
}

/// Links an OpenGL program.
void CShader::LinkProgram(GLuint program)
{
	GLint tmp = GL_TRUE;
    glLinkProgram(mProgram);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &tmp);
    if(tmp == GL_FALSE)
    {
    	char infolog[501];
    	int length;
    	printf("Could not build mProgram!");
    	glGetProgramInfoLog(mProgram, 500, &length, infolog);
    	printf("%s\n", infolog);
    }
}

void CShader::UseShader()
{
	if(!mShaderLoaded)
		Init();

	// Tell OpenGL to use the mProgram
	glUseProgram(mProgram);
	CWorkerThread::CheckOpenGLError("CShader::UseShader, glUseProgram");

	// Set the shader-specific parameters.  Notice again the intentional downcast.
	GLfloat tmp;
	unsigned int i = 0;
	for(auto it: mParams)
	{
		tmp = GLfloat(it.second.getValue());
		cout << "Value: " << tmp << endl;
		glUniform1fv(mParam_locations[i], 1, &tmp);
		i++;
	}

	CWorkerThread::CheckOpenGLError("CShader::UseShader, glUniform1fv");
}

//void CShader::UseShader(double * params, unsigned int in_params)
//{
//	if(!mShaderLoaded)
//		Init();
//
//	// Tell OpenGL to use the mProgram
//	glUseProgram(mProgram);
//	CWorkerThread::CheckOpenGLError("CShader::UseShader, glUseProgram");
//
//	// Set the shader-specific parameters.  Notice again the intentional downcast.
//	GLfloat tmp;
//	unsigned int nParams = mParams.size();
//	for(int i = 0; (i < nParams && i < in_params); i++)
//	{
//		tmp = GLfloat(params[i]);
//		glUniform1fv(mParam_locations[i], 1, &tmp);
//	}
//	CWorkerThread::CheckOpenGLError("CShader::UseShader, glUniform1fv");
//}
