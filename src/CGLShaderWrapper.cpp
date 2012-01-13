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
	mShader = shader;
	mNParams = n_params;
	mParams = new float[n_params];

	// Tell the shader to compile
	shader->Init();
}

CGLShaderWrapper::~CGLShaderWrapper()
{
	// Free memory.
	delete mParams;

	// Set shader to null.  Don't delete it, the CGLShaderList takes care of that.
	mShader = NULL;
}

// Gets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CGLShaderWrapper::GetParams(float * in_params, int n_params)
{
	for(int i = 0; i < mNParams; i++)
		in_params[i] = mParams[i];
}

int CGLShaderWrapper::GetNFreeParams()
{
	return mNParams;
}

// Sets the parameters.
// Note, the input doubles are downcast to floats to be compatable with OpenGL function calls later.
void CGLShaderWrapper::SetParams(float * in_params, int n_params)
{
	for(int i = 0; i < mNParams; i++)
		mParams[i] = (float) in_params[i];
}

void CGLShaderWrapper::UseShader()
{
	if(mShader != NULL)
		mShader->UseShader(mParams, mNParams);
}
