/*
 * CGLShaderWrapper.cpp
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
void CGLShaderWrapper::UseShader(double min_xyz[3], double max_xyz[3])
{
	if(mShader != NULL)
		mShader->UseShader(min_xyz, max_xyz, mParams, mNParams);
}
