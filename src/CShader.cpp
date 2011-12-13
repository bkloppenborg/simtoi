/*
 * CShader.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CShader.h"
#include "CGLShader.h"

CShader::CShader(CGLShader * shader, int n_params)
{
	this->shader = shader;
	this->n_params = n_params;
	this->params = new float[n_params];
}

CShader::~CShader()
{
	// Free memory.
	delete params;

	// Set shader to null.  Don't delete it, the CGLShaderList takes care of that.
	params = NULL;
}

// Gets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CShader::GetParams(float * in_params, int n_params)
{
	for(int i = 0; i < this->n_params; i++)
		in_params[i] = params[i];
}

int CShader::GetNFreeParams()
{
	return this->n_params;
}

// Sets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CShader::SetParams(float * in_params, int n_params)
{
	for(int i = 0; i < n_params; i++)
		params[i] = (float) in_params[i];
}

void CShader::UseShader()
{
	if(shader != NULL)
		shader->UseShader(params, n_params);
}
