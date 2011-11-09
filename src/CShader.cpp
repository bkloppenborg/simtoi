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

// Sets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CShader::SetParams(double * in_params, int n_params)
{
	for(int i = 0; i < n_params; i++)
		params[i] = (float) in_params[i];
}

void CShader::UseShader()
{
    shader->UseShader(params, n_params);
}
