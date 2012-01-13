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
#include "CGLThread.h"

CGLShader::CGLShader(eGLShaders type, string shader_dir, string base_filename, int n_parameters, vector<string> parameter_names)
{
	this->type = type;
	this->shader_dir = shader_dir;
	this->base_name = base_filename;
	this->n_params = n_parameters;
	this->param_names = parameter_names;
	param_locations = new GLuint[n_parameters];
	mShaderLoaded = false;
}

CGLShader::~CGLShader()
{
	// Release OpenGL memory
	if(program) glDeleteProgram(program);
	if(shader_vertex) glDeleteShader(shader_vertex);
	if(shader_fragment) glDeleteShader(shader_fragment);

	// Now release object memory:
	delete param_locations;
}

int CGLShader::GetNParams()
{
	return this->n_params;
}

eGLShaders CGLShader::GetType()
{
	return this->type;
}

// Loads the shader from the source file and creates a binary for the current selected context.
void CGLShader::Init()
{
	// If the shader is loaded, immediately exit the function
	if(mShaderLoaded)
		return;

    // First load the program source.
    string source_v, source_f;
	const GLchar * tmp_source_v;
	const GLchar * tmp_source_f;
    source_v = ReadFile(shader_dir + '/' + base_name + ".vert", "Could not read " + base_name + ".vert file!");
    source_f = ReadFile(shader_dir + '/' + base_name + ".frag", "Could not read " + base_name + ".frag file!");
	tmp_source_v = (const GLchar *) source_v.c_str();
	tmp_source_f = (const GLchar *) source_f.c_str();

    // Now create the program with vertex and fragement shaders
	program = glCreateProgram();
    CGLThread::CheckOpenGLError("Could not create shader program.");
    shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(program, shader_vertex);
    glAttachShader(program, shader_fragment);

    // Now put the shader code into the object, compile and link.
    glShaderSource(shader_vertex, 1, &tmp_source_v, NULL);
    glShaderSource(shader_fragment, 1, &tmp_source_f, NULL);

    glCompileShader(shader_vertex);
    glCompileShader(shader_fragment);
    glLinkProgram(program);

    GLint tmp;
    glGetProgramiv(program, GL_LINK_STATUS, &tmp);
    if(tmp == GL_FALSE)
    {
    	char * infolog = (char*) malloc(501);
    	int length;
    	printf("Could not build program!");
    	glGetProgramInfoLog(program, 500, &length, infolog);
    	printf("%s\n", infolog);
    }

    CGLThread::CheckOpenGLError("Could not link shader program.");

    // Now look up the locations of the parameters
    for(int i = 0; i < n_params; i++)
    {
    	param_locations[i] = glGetUniformLocation(program, param_names[i].c_str());
    	CGLThread::CheckOpenGLError("Could find variable in shader source.");
    }

    // The shader has been loaded, compiled, and linked.
    mShaderLoaded = true;
}

void CGLShader::UseShader(float * params, int in_params)
{
	//if(in_params != this->n_params)
	// throw exception

	// Tell OpenGL to use the program
	glUseProgram(program);

	// Set the parameters.
	for(int i = 0; i < n_params; i++)
		glUniform1fv(param_locations[i], 1, &params[i]);
}
