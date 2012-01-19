/*
 * CGLShaderWrapper.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CGLShader.h"
#include "CGLShaderWrapper.h"

CGLShaderWrapper::CGLShaderWrapper(CGLShader * shader, int n_params)
	: CParameters(n_params)
{
	// Set variables
	mShader = shader;

	// Tell the shader to compile
	shader->Init();
}

CGLShaderWrapper::~CGLShaderWrapper()
{
	// Set shader to null.  Don't delete it, the CGLShaderList takes care of that.
	mShader = NULL;
}

// Executes the OpenGL shader
void CGLShaderWrapper::UseShader()
{
	if(mShader != NULL)
		mShader->UseShader(mParams, mNParams);
}
