/*
 * CGLShaderWrapper.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CGLShader.h"
#include "CGLShaderWrapper.h"
#include "CModel.h"

CGLShaderWrapper::CGLShaderWrapper(CGLShader * shader, int n_params)
	: CParameters(n_params)
{
	// Set variables
	mShader = shader;
	mName = shader->GetName();

	// Now populate the shader parameters
	for(int i = 0; i < n_params; i++)
	{
		mParamNames.push_back(shader->GetParamName(i));
		mParams[i] = shader->GetStartingValue(i);
		mMinMax[i].first = shader->GetMin(i);
		mMinMax[i].second = shader->GetMax(i);
	}

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
