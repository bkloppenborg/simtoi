/*
 * CParameters.cpp
 *
 *  Created on: Jan 18, 2012
 *      Author: bkloppenborg
 */

#include "CParameters.h"
#include <cmath>

#include "misc.h"	// needed for pull_params

// A number after which we consider a parameter to be zero.
#define ZERO_COMP 1E-8

CParameters::CParameters(int n_params)
{
	// Init the parameter storage location
	mNParams = n_params;
	mNFreeParams = 0;
	mParams = new float[mNParams];
	mFreeParams = new bool[mNParams];
	mScales = new float[mNParams];
	mMinMax = new pair<float,float>[mNParams];
	mName = "";

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = 0;
		mFreeParams[i] = false;
		mScales[i] = 1;
		mMinMax[i] = pair<float,float>(0.0, 0.0);
	}
}

CParameters::~CParameters()
{
	delete[] mParams;
	delete[] mFreeParams;
	delete[] mScales;
	delete[] mMinMax;
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

/// Returns the maximum allowable value of the specified parameter, -1 if param_num is out of bounds.
float CParameters::GetMax(int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].second;

	return -1;
}

/// Returns the minimum allowable value of the specified parameter, -1 if param_num is out of bounds.
float CParameters::GetMin(int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].first;

	return -1;
}

/// Gets the vale of the specified parameter, returns -1 if outside of bounds:
float CParameters::GetParam(int param_n)
{
	if(param_n < mNParams)
		return mParams[param_n];

	return -1;
}

/// Gets the values of the (scaled) parameters for this object.
void CParameters::GetParams(float * out_params, int n_params)
{
	// Copy the current parameter value into out_params
	// keep in bounds for both n_params and mNParams.
	for(unsigned int i = 0; (i < n_params && i < mNParams); i++)
	{
		out_params[i] = mParams[i];
	}
}

/// Returns a vector of pairs containing the min/max values for the parameters.
vector< pair<float, float> > CParameters::GetFreeMinMaxes()
{
	vector< pair<float, float> > tmp;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			tmp.push_back(mMinMax[i]);
	}

	return tmp;
}

/// Gets the values of the free parameters for this object, scales them into
/// a uniform hypercube [0...1]
void CParameters::GetFreeParams(float * out_params, int n_params)
{
	GetFreeParamsScaled(out_params, n_params);

	// Scale the parameters into a unit hypercube.
	// f(x) = (x - min) / (max - min) // the scaled value
	// x = f(x) * (max - min) + min   // restore the original value, see SetFreeParams

	int j = 0;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
		{
			out_params[j] = (out_params[j] - mMinMax[i].first) /  (mMinMax[i].second - mMinMax[i].first);
			j++;
		}
	}
}

/// Gets the values of the free parameters for this object, returning them in their native (scaled) units.
void CParameters::GetFreeParamsScaled(float * out_params, int n_params)
{
	pull_params(mParams, mNParams, out_params, n_params, mFreeParams);
}

/// Returns a vector of strings containing the names of the free parameters.
vector<string> CParameters::GetFreeParamNames()
{
	vector<string> tmp;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			tmp.push_back(GetParamName(i));
	}

	return tmp;
}

/// Returns all of the parameter names as a vector of strints
vector<string> CParameters::GetParamNames()
{
	return mParamNames;
}

/// Returns all of the parameters as a vector of pairs in (id, name) format
vector< pair<int, string> > CParameters::GetParamIDsNames()
{
	vector< pair<int, string> > tmp;

	for(int i = 0; i < mNParams; i++)
		tmp.push_back(pair<int, string> (i, mParamNames[i]));

	return tmp;
}

string CParameters::GetParamName(unsigned int i)
{
	if(i < mParamNames.size())
		return mParamNames[i];

	return "! NotFound !";
}

bool CParameters::IsFree(int param_num)
{
	if(param_num < mNParams)
		return mFreeParams[param_num];

	return false;
}

/// Toggles the state of all variables to free (is_free = true) or fixed (is_free = false)
void CParameters::SetAllFree(bool is_free)
{
	for(int i = 0; i < mNParams; i++)
		mFreeParams[i] = is_free;

	CountFree();
}

/// Sets the values for the parameters for this object scaling them as necessary.
void CParameters::SetFreeParams(float * in_params, int n_params)
{
	// Set the parameter values
	push_params(in_params, n_params, mParams, mNParams, mFreeParams);

	// Scale the parameters into regular units
	// f(x) = (x - min) / (max - min) // the scaled value (see GetFreeParams)
	// x = f(x) * (max - min) + min   // restore the original value.
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			mParams[i] = (mMinMax[i].second - mMinMax[i].first) * mParams[i] + mMinMax[i].first;
	}
}

/// Sets the specified parameter as free (is_free = true) or fixed (is_free = false)
void CParameters::SetFree(int param_num, bool is_free)
{
	if(param_num <= mNParams)
		mFreeParams[param_num] = is_free;

	// Update the number of free parameters
	CountFree();
}

/// Sets the specified parameter's minimum value.
void CParameters::SetMin(int param_num, float value)
{
	if(param_num < mNParams && value < mMinMax[param_num].second)
		mMinMax[param_num].first = value;
}

/// Sets the specified paramter's maximum value.
void CParameters::SetMax(int param_num, float value)
{
	if(param_num < mNParams && value > mMinMax[param_num].first)
		mMinMax[param_num].second = value;
}

/// Sets the specified parameter to the indicated value.
/// Note, scaling from the unit hypercube is not applied.
void CParameters::SetParam(int n_param, float value)
{
	if(n_param < mNParams)
		mParams[n_param] = value;
}

