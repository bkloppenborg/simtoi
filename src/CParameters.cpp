/*
 * CParameters.cpp
 *
 *  Created on: Jan 18, 2012
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

#include "CParameters.h"
#include <cmath>

#include "misc.h"	// needed for pull_params

// TODO: Note, none of the store/set operations here are thread safe!

// A number after which we consider a parameter to be zero.
#define ZERO_COMP 1E-8

CParameters::CParameters(const CParameters & other)
{
	// Init the parameter storage location
	mNParams = other.mNParams;
	mNFreeParams = other.mNFreeParams;
	mParams = new double[mNParams];
	mFreeParams = new bool[mNParams];
	mScales = new double[mNParams];
	mMinMax = new pair<double,double>[mNParams];
	mName = other.mName;

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = other.mParams[i];
		mFreeParams[i] = other.mFreeParams[i];
		mScales[i] = other.mScales[i];
		mMinMax[i] = pair<double,double>(0.0, 0.0);
		mMinMax[i].first = other.mMinMax[i].first;
		mMinMax[i].second = other.mMinMax[i].second;
		mParamNames.push_back(other.mParamNames[i]);
	}
}

CParameters::CParameters(unsigned int n_params)
{
	// Init the parameter storage location
	mNParams = n_params;
	mNFreeParams = 0;
	mParams = new double[mNParams];
	mFreeParams = new bool[mNParams];
	mScales = new double[mNParams];
	mMinMax = new pair<double,double>[mNParams];
	mName = "";

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = 0;
		mFreeParams[i] = false;
		mScales[i] = 1;
		mMinMax[i] = pair<double,double>(0.0, 0.0);
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
double CParameters::GetMax(unsigned int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].second;

	return -1;
}

/// Returns the minimum allowable value of the specified parameter, -1 if param_num is out of bounds.
double CParameters::GetMin(unsigned int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].first;

	return -1;
}

/// Gets the vale of the specified parameter, returns -1 if outside of bounds:
double CParameters::GetParam(unsigned int param_n)
{
	if(param_n < mNParams)
		return mParams[param_n];

	return -1;
}

/// Gets all of the parameter values for this object, returning them in intervals x_i = [param[i].min ... param[i].max].
void CParameters::GetParams(double * out_params, unsigned int n_params)
{
	// Copy the current parameter value into out_params
	// keep in bounds for both n_params and mNParams.
	for(unsigned int i = 0; (i < n_params && i < mNParams); i++)
	{
		out_params[i] = mParams[i];
	}
}

/// Returns the prior for the i-th parameter:
/// NOTE: at the moment all parameters have uniform priors.
double CParameters::GetPrior(unsigned int i)
{
	// compute a uniform prior.
	return 1.0 / (mMinMax[i].second - mMinMax[i].first);
}

/// Returns a vector of pairs containing the min/max values for the parameters.
vector< pair<double, double> > CParameters::GetFreeMinMaxes()
{
	vector< pair<double, double> > tmp;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			tmp.push_back(mMinMax[i]);
	}

	return tmp;
}

/// Gets the values of the free parameters for this object, scales them into
// If scale_params = false, parameters are returned within the interval x = [0...1] otherwise, x = [param.min ... param.max]
void CParameters::GetFreeParams(double * out_params, unsigned int n_params, bool scale_params)
{
	// Get the scaled parameter values
	pull_params(mParams, mNParams, out_params, n_params, mFreeParams);

	// if we want scaled values, return, otherwise cast them into the interval [0...1]
	if(scale_params)
		return;

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

/// Returns the product of the priors for the free parameters
double CParameters::GetFreePriorProd()
{
	double tmp = 1;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			tmp *= GetPrior(i);
	}

	return tmp;
}

/// Returns a vector of strings containing the names of the free parameters
/// prefixed with the name of the parent object.
vector<string> CParameters::GetFreeParamNames()
{
	vector<string> tmp;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			tmp.push_back(mName + '.' + GetParamName(i));
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

bool CParameters::IsFree(unsigned int param_num)
{
	if(param_num < mNParams)
		return mFreeParams[param_num];

	return false;
}

/// Restores parameters values from the JSON value
void CParameters::Restore(Json::Value input)
{
	// Restore parameters.  Check that the parameter is set in the file before attempting
	// to restore it.
	string name;
	for(unsigned int i = 0; i < mNParams; i++)
	{
		name = mParamNames[i];
		if(input.isMember(name))
		{
			//
			mParams[i] =        double( input[name][0u].asDouble() );
			mMinMax[i].first =  double( input[name][1u].asDouble() );
			mMinMax[i].second = double( input[name][2u].asDouble() );
			mFreeParams[i] =    bool ( input[name][3u].asBool()   );
		}
	}

	CountFree();
}

/// Toggles the state of all variables to free (is_free = true) or fixed (is_free = false)
void CParameters::SetAllFree(bool is_free)
{
	for(int i = 0; i < mNParams; i++)
		mFreeParams[i] = is_free;

	CountFree();
}

/// Sets the values for the parameters for this object
/// If scale_params = true then the values are scaled into their native interval, [param.min ... param.max],
/// otherwise if scale_params = false the parameters are assumed to be scaled.
void CParameters::SetFreeParams(double * in_params, unsigned int n_params, bool scale_params)
{
	// Set the parameter values
	push_params(in_params, n_params, mParams, mNParams, mFreeParams);

	// If the parameters do not need to be scaled, we are done.
	if(!(scale_params))
		return;

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
void CParameters::SetFree(unsigned int param_num, bool is_free)
{
	if(param_num <= mNParams)
		mFreeParams[param_num] = is_free;

	// Update the number of free parameters
	CountFree();
}

/// Sets the specified parameter's minimum value.
void CParameters::SetMin(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mMinMax[param_num].first = value;
}

/// Sets the specified paramter's maximum value.
void CParameters::SetMax(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mMinMax[param_num].second = value;
}

/// Sets the specified parameter to the indicated value.
/// Note, scaling from the unit hypercube is not applied.
void CParameters::SetParam(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mParams[param_num] = value;
}

/// Serializes the parameters to a Json::Value object
Json::Value CParameters::Serialize()
{
	Json::Value output;

	for(unsigned int i = 0; i < mNParams; i++)
	{
		// Write out the parameters in [name, value, min, max] format
		Json::Value tmp;
		tmp.append(Json::Value(mParams[i]));
		tmp.append(Json::Value(mMinMax[i].first));
		tmp.append(Json::Value(mMinMax[i].second));
		tmp.append(Json::Value(mFreeParams[i]));
		output[mParamNames[i]] = tmp;
	}

	return output;
}
