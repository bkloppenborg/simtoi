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
#include <stdexcept>

#include "misc.h"	// needed for pull_params

// A number after which we consider a parameter to be zero.
#define ZERO_COMP 1E-8

/// \brief Copy constructor
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
	mSteps = new double[mNParams];

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
		mSteps[i] = other.mSteps[i];
	}
}

/// \brief Default constructor
///
/// Construct a CParameters object of size n_params.
/// \param n_params The number of parameters for this object.
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
	mSteps = new double[mNParams];

	// Init parameter values.
	for(int i = 0; i < mNParams; i++)
	{
		mParams[i] = 0;
		mFreeParams[i] = false;
		mScales[i] = 1;
		mMinMax[i] = pair<double,double>(0.0, 0.0);
		mSteps[i] = 0;
	}
}

CParameters::~CParameters()
{
	delete[] mParams;
	delete[] mFreeParams;
	delete[] mScales;
	delete[] mMinMax;
	delete[] mSteps;
}

/// \brief Counts the number of free parameters in this object
/// 	and stores the result in mNFreeParams.
void CParameters::CountFree(void)
{
	mNFreeParams = 0;
	for(int i = 0; i < mNParams; i++)
	{
		if(mFreeParams[i])
			mNFreeParams += 1;
	}
}

/// \brief Returns the maximum allowable value of the specified parameter.
///
/// If the parameter is out of bounds, this function throws a out_of_range
/// exception.
double CParameters::GetMax(unsigned int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].second;

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::GetMax().");

	return 0;
}

/// \brief Returns the minimum allowable value of the specified parameter.
///
/// If the parameter is out of bounds, this function throws a out_of_range
/// exception.
double CParameters::GetMin(unsigned int param_num)
{
	if(param_num < mNParams)
		return mMinMax[param_num].first;

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::GetMin().");

	return 0;
}

/// \brief Returns the value of the specified parameter.
///
/// If the parameter is out of bounds, this function throws a out_of_range
/// exception.
double CParameters::GetParam(unsigned int param_n)
{
	if(param_n < mNParams)
		return mParams[param_n];

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::GetMax().");

	return 0;
}

/// \brief Gets all of the parameter values for this object
///
/// Copies up to `n_params` parameter values from this object to `out_params`.
/// Values are returned in the interval x_i = [param[i].min ... param[i].max].
/// \param out_params A pre-allocated array into which values are copied.
/// \param n_params The size of out_params.
void CParameters::GetParams(double * out_params, unsigned int n_params)
{
	// Copy the current parameter value into out_params
	// keep in bounds for both n_params and mNParams.
	for(unsigned int i = 0; (i < n_params && i < mNParams); i++)
	{
		out_params[i] = mParams[i];
	}
}

/// \brief Returns a vector of pairs containing the min/max values for
/// 	the free parameters.
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

/// \brief Gets the values of the free parameters for this object
///
/// Copies up to `n_params` parameter values from this object into `out_params`
///
/// \param out_params A pre-allocated buffer into which parameter values are placed
/// \param n_params The size of `out_params`
/// \param scale_params If true, parameters are returned in the interval
///		x = [param.min ... param.max]. Otherwise, parameters are returned on a unit
/// 	hypercube x = [0...1].
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


/// \brief Returns a vector of strings containing the names of the free parameters
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

/// \brief Returns all of the parameter names as a vector of strings
vector<string> CParameters::GetParamNames()
{
	return mParamNames;
}

void CParameters::GetFreeParamSteps(double * steps, unsigned int size)
{
	int j = 0;
	for(int i = 0; i < mNParams && j < size; i++)
	{
		if(mFreeParams[i])
		{
			steps[j] = mSteps[i];
			j++;
		}
	}
}

/// \brief Returns all of the parameters as a vector of pairs in (id, name) format
vector< pair<int, string> > CParameters::GetParamIDsNames()
{
	vector< pair<int, string> > tmp;

	for(int i = 0; i < mNParams; i++)
		tmp.push_back(pair<int, string> (i, mParamNames[i]));

	return tmp;
}

/// \brief Returns the name of the specified parameter.
///
/// If `param_num` is out of bounds, an `out_of_range` exception will be thrown.
string CParameters::GetParamName(unsigned int param_n)
{
	if(param_n < mParamNames.size())
		return mParamNames[param_n];

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::GetParamName().");

	return 0;
}

/// \brief Returns the value of the specified parameter.
///
/// If the parameter is out of bounds, this function throws a out_of_range
/// exception.
double CParameters::GetStepSize(unsigned int param_n)
{
	if(param_n < mNParams)
		return mSteps[param_n];

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::GetStepSize().");

	return 0;
}

/// \brief Returns true of the specified parameter is free.
///
/// If `param_num` is out of bounds, an `out_of_range` exception will be thrown.
bool CParameters::IsFree(unsigned int param_num)
{
	if(param_num < mNParams)
		return mFreeParams[param_num];

	throw std::out_of_range("Attempt to access an out-of-bound parameter. CParameters::IsFree().");

	return false;
}

/// \brief Restores parameters values from the JSON value
///
/// Restores parameter values, names, and min/max values from a SIMTOI JSON save file.
/// Parameters are imported by the name specified in `mParamNames`. If the parameter
/// is not found in the JSON file, the default values provided in the `CParameters::CParameters()`
/// constructor are left intact.
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
			// Note, we do not need to check the sizes of input[name] here
			// because JSONcpp will automatically fill in zeros for undefined
			// values.
			mParams[i] =        double( input[name][0u].asDouble() );
			mMinMax[i].first =  double( input[name][1u].asDouble() );
			mMinMax[i].second = double( input[name][2u].asDouble() );
			mFreeParams[i] =    bool  ( input[name][3u].asBool()   );
			mSteps[i] = 		double( input[name][4u].asDouble() );
		}
	}

	CountFree();
}

/// \brief Toggles the state of all variables to free or fixed.
///
/// \param is_free If true, all parameters in this object are set to free.
///		If false, all parameters will be regarded as fixed.
void CParameters::SetAllFree(bool is_free)
{
	for(int i = 0; i < mNParams; i++)
		mFreeParams[i] = is_free;

	CountFree();
}

/// \brief Sets the values for the parameters for this object.
///
/// Sets the values of the parameters in this object to the corresponding
/// values in `in_params`. By default, parameters are expected to be in native
/// units (i.e. x_i = [param.min ... param.max]; however, scaling can be done
/// inside of this object if needed by setting `scale_params = true`. Scaling
/// happens via. linear interpolation between the minimum and maximum values
/// specified for the parameter.
///
/// \param in_params The values to be set
/// \param n_params The size of `in_params`
/// \param scale_params If true, the values in `in_params` are on a unit hypercube
///		and need to be scaled to native values.
void CParameters::SetFreeParams(double * in_params, unsigned int n_params, bool scale_params = false)
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

/// \brief Sets the specified parameter as free (is_free = true) or fixed (is_free = false)
void CParameters::SetFree(unsigned int param_num, bool is_free)
{
	if(param_num <= mNParams)
		mFreeParams[param_num] = is_free;

	// Update the number of free parameters
	CountFree();
}

/// \brief Sets the specified parameter's minimum value.
void CParameters::SetMin(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mMinMax[param_num].first = value;
}

/// \brief Sets the specified paramter's maximum value.
void CParameters::SetMax(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mMinMax[param_num].second = value;
}

/// \brief Sets the specified parameter to the indicated value.
///
/// NOTE: It is expected value is in native units. Scaling from a unit hypercube
/// is not performed
void CParameters::SetParam(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mParams[param_num] = value;
}

/// \brief Sets the specified step size to the indicated value.
///
/// NOTE: It is expected value is in native units. Scaling from a unit hypercube
/// is not performed
void CParameters::SetStepSize(unsigned int param_num, double value)
{
	if(param_num < mNParams)
		mSteps[param_num] = value;
}

/// \brief Serializes the parameters to a Json::Value object
///
/// Copies the parameter value, minimum/maximum, and free state into a Json::Value
/// object in the format: `[name, value, min, max]`.
///
/// NOTE: This method can be overridden; however, it is suggested that this function
/// be called by any overriding classes to ensure proper object serialization.
/// For example, see `CModel::Serialize`.
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
		tmp.append(Json::Value(mSteps[i]));
		output[mParamNames[i]] = tmp;
	}

	return output;
}
