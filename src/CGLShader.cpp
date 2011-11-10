/*
 * CGLShader.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CGLShader.h"
#include "ReadTextFile.h"

CGLShader::CGLShader()
{
	// TODO Auto-generated constructor stub

}

CGLShader::~CGLShader()
{
	// Release OpenGL memory
	glDeleteProgram(program);
	glDeleteShader(shader_vertex);
	glDeleteShader(shader_fragment);

	// Now release object memory:
	delete param_locations;
}

// Loads the shader from the source file, links
void CGLShader::init_shader()
{
    // Load some shaders:
    program = glCreateProgram();

    shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glAttachShader(program, shader_vertex);
    glAttachShader(program, shader_fragment);

    string source_v, source_f;
    source_v = ReadFile("../src/shaders/" + base_name + ".vert", "Could not read " + base_name + ".vert file!");
    source_f = ReadFile("../src/shaders/" + base_name + ".frag", "Could not read " + base_name + ".frag file!");

	const GLchar * tmp_source_v;
	const GLchar * tmp_source_f;

	tmp_source_v = (const GLchar *) source_v.c_str();
	tmp_source_f = (const GLchar *) source_v.c_str();

    // Now put the shader code into the object, compile and link.
    glShaderSource(shader_vertex, 1, &tmp_source_v, NULL);
    glShaderSource(shader_fragment, 1, &tmp_source_f, NULL);


    glCompileShader(shader_vertex);
    glCompileShader(shader_fragment);
    glLinkProgram(program);

    // Now look up the locations of the parameters
    for(int i = 0; i < n_params; i++)
    	param_locations[i] = glGetUniformLocation(program, param_names[i].c_str());


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
