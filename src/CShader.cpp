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
	: CParameters(other)
{
	mShaderID = other.mShaderID;
	mShader_dir = other.mShader_dir;
	mVertShaderFileName = other.mVertShaderFileName;
	mFragShaderFilename = other.mFragShaderFilename;
	mNParams = other.mNParams;
	mParam_locations = new GLuint[mNParams];
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;

	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = other.mParams[i];
		mMinMax[i].first = other.mMinMax[i].first;
		mMinMax[i].second = other.mMinMax[i].second;
		mSteps[i] = other.mSteps[i];
	}
}

// Constructor from JSON save file
CShader::CShader(string json_config_file)
	: CParameters(0)
{
	// NOTE: because we do not know the number of parameters on construction
	// we have to initialize all CParameter base variables here too.

	// TODO: Try to simplify this constructor. It's a mess.

	// Read in the JSON configuration file:
	Json::Reader reader;
	Json::Value input;
	string file_contents = ReadFile(json_config_file, "Could not read shader save file: '" + json_config_file + "'.");
	bool parsingSuccessful = reader.parse(file_contents, input);
	if(!parsingSuccessful)
		throw runtime_error("JSON parse error in shader configuration file '" + json_config_file + "'. Shader not loaded.");

	// Now read in the parameters from the file
	stringstream tmp;
	string id = "";
	vector<string> t_names;
	vector<pair<float, float>> t_min_max;
	vector<float> t_start_vals;
	for(int i = 0; true; i++)
	{
		// reset the stringstream
		tmp.str("");
		tmp.clear();
		tmp << "param_" << i;
		id = tmp.str();
		if(!input.isMember(id))
			break;

		t_names.push_back(input[id]["name"].asString());
		t_min_max.push_back( pair<float, float>(input[id]["min"].asDouble(), input[id]["max"].asDouble()) );
		t_start_vals.push_back(input[id]["default"].asDouble());
	}

	//
	// Initialize base class variables.
	//
	mNParams = t_names.size();
	mNFreeParams = 0;
	mParams = new double[mNParams];
	mFreeParams = new bool[mNParams];
	mScales = new double[mNParams];
	mMinMax = new pair<double,double>[mNParams];
	mName = input["human_name"].asString();
	mSteps = new double[mNParams];

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = t_start_vals[i];
		mFreeParams[i] = false;
		mScales[i] = 1;
		mMinMax[i] = pair<double,double>(0.0, 0.0);
		mMinMax[i].first = t_min_max[i].first;
		mMinMax[i].second = t_min_max[i].second;
		mParamNames.push_back(t_names[i]);
		mSteps[i] = 0;
	}

	//
	// Init remaining CShader members
	//
	mParam_locations = new GLuint[mNParams];
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;
	// Now init the datamembers.
	mShaderID = input["shader_id"].asString();
	// Shaders and configuration files must be in the same directory.
	size_t folder_end = json_config_file.find_last_of("/\\");
	mShader_dir = json_config_file.substr(0,folder_end+1);
	mVertShaderFileName = input["vertex_shader"].asString();
	mFragShaderFilename = input["fragment_shader"].asString();
}

CShader::CShader(string shader_id, string shader_dir, string vertex_shader_filename, string fragment_shader_filename,
		string friendly_name, int n_parameters,
		vector<string> parameter_names, vector<float> starting_values, vector< pair<float, float> > minmax)
	: CParameters(0)
{
	mShaderID = shader_id;
	mShader_dir = shader_dir;
	mVertShaderFileName = vertex_shader_filename;
	mFragShaderFilename = fragment_shader_filename;
	mParam_locations = new GLuint[mNParams];
	mShaderLoaded = false;
	mProgram = 0;
	mShader_vertex = 0;
	mShader_fragment = 0;
	mName = friendly_name;

	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = starting_values[i];
		mMinMax[i].first = minmax[i].first;
		mMinMax[i].second = minmax[i].second;
		mSteps[i] = 0;
	}
}

CShader::~CShader()
{
	// Release OpenGL memory
	if(mProgram) glDeleteProgram(mProgram);
	if(mShader_vertex) glDeleteShader(mShader_vertex);
	if(mShader_fragment) glDeleteShader(mShader_fragment);

	// Now release object memory:
	delete[] mParam_locations;
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
    for(int i = 0; i < mNParams; i++)
    {
    	mParam_locations[i] = glGetUniformLocation(mProgram, mParamNames[i].c_str());
    	CWorkerThread::CheckOpenGLError("Could find variable in shader source.");
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
