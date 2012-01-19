/*
 * CParameters.cpp
 *
 *  Created on: Jan 18, 2012
 *      Author: bkloppenborg
 */

#include "CParameters.h"
#include "misc.h"	// needed for pull_params

CParameters::CParameters(int n_params)
{
	// Init the parameter storage location (+3 because yaw, pitch, and roll are included)
	mNParams = n_params;
	mNFreeParams = 0;
	mParams = new float[mNParams];
	mFreeParams = new bool[mNParams];
	mScales = new float[mNParams];
	mScales_min = new float[mNParams];

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = 0;
		mFreeParams[i] = false;
		mScales[i] = 1;
		mScales_min[i] = 0;
	}
}

CParameters::~CParameters()
{
	delete mParams;
	delete mFreeParams;
	delete mScales;
	delete mScales_min;
}

/// Counts the number of free parameters, sets that value to mNFreeParams
void CParameters::CountFree(void)
{
	mNFreeParams = 0;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			mNFreeParams += 1;
	}
}

/// Gets the values of the (scaled) parameters for this object.
void CParameters::GetParams(float * out_params, int n_params)
{
	pull_params(mParams, mNParams, out_params, n_params, mFreeParams);
}

vector<string> CParameters::GetParamNames()
{
	return mParamNames;
}

string CParameters::GetParamName(int i)
{
	if(i < mParamNames.size())
		return mParamNames[i];
}

/// Sets the specified parameter as free (is_free = true) or fixed (is_free = false)
void CParameters::SetFree(int param_num, bool is_free)
{
	if(param_num <= mNParams)
		mFreeParams[param_num] = is_free;

	// Update the number of free parameters
	CountFree();
}

/// Sets the values for the parameters for this object scaling them as necessary.
void CParameters::SetParams(float * in_params, int n_params)
{
	// Set the parameter values
	pull_params(in_params, n_params, mParams, mNParams, mFreeParams);

	// Now scale the parameters (only free parameters could have changed, so just scale those)
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			mParams[i] = mScales_min[i] + mScales[i] * mParams[i];
	}
}
