/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 */

#include "CMinimizer.h"

CMinimizer::CMinimizer(MinimizationType type)
{
	mType = type;

}

CMinimizer::~CMinimizer()
{
	delete mParamBounds;
}

void CMinimizer::SetParamBounds(float * bounds, int n_params)
{
	// Allocate memory and then copy the bounds into a class-level variable for later reference.
	mParamBounds = new float[2*n_params];

	for(int i = 0; i < n_params; i++)
	{
		mParamBounds[2*i] = bounds[2*i];
		mParamBounds[2*i+1] = bounds[2*i+1];
	}
}
