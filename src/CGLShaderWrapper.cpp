/*
 * CGLShaderWrapper.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CGLShader.h"
#include "CGLShaderWrapper.h"

CGLShaderWrapper::CGLShaderWrapper(CGLShader * shader, int n_params)
{
	// Set variables
	this->shader = shader;
	this->n_params = n_params;
	this->params = new float[n_params];

	// Tell the shader to compile
	shader->Init();
}

CGLShaderWrapper::~CGLShaderWrapper()
{
	// Free memory.
	delete params;

	// Set shader to null.  Don't delete it, the CGLShaderList takes care of that.
	shader = NULL;
}

// Gets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CGLShaderWrapper::GetParams(float * in_params, int n_params)
{
	for(int i = 0; i < this->n_params; i++)
		in_params[i] = params[i];
}

int CGLShaderWrapper::GetNFreeParams()
{
	return this->n_params;
}

// Sets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CGLShaderWrapper::SetParams(float * in_params, int n_params)
{
	for(int i = 0; i < n_params; i++)
		params[i] = (float) in_params[i];
}

void CGLShaderWrapper::UseShader()
{
	if(shader != NULL)
		shader->UseShader(params, n_params);
}
