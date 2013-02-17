/*
 * CParameters.h
 *
 *  Created on: Jan 18, 2012
 *      Author: bkloppenborg
 *
 *  Basic class for storing, setting, and getting parameters.
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

#ifndef CPARAMETERS_H_
#define CPARAMETERS_H_

#include <vector>
#include <string>
#include <utility>
#include "json/json.h"

using namespace std;

/// \brief A base class for storing parameter values, names, minimum and maximums.
///
/// The `CParameters` class is a common storage container for objects which
/// have parameters. Examples are the `CModel`, `CPosition`, and `CShader` objects.
///
/// This class regards parameters as either fixed or free. Although both types of
/// parameters can be queried/manipulated using the `[Set|Get]Param` functions,
/// the free parameters are most often changed using an external minimization
/// process.
///
/// To assist with minimization, the routines herein can provide parameters
/// in their native units (i.e. min <= param <= max) or as values on a unit
/// hypercube (e.g. 0 <= param <= 1) by specifying the scaling option in
/// `GetFreeParams` and `SetFreeParams`. All other Set/Get methods expect
/// that parameters are in their native units.
///
/// NOTE: None of the set/get operations are intended to be thread safe.
/// You must ensure thread saftey externally.
class CParameters
{
protected:
	// Varaibles for parameter values
	int mNParams; 			///< The number of parameters stored in this object.
	double * mParams; 		///< The parameters
	int mNFreeParams; 		///< The number of free parameters stored in this object
	bool * mFreeParams; 	///< A bitmask for which parameters are free
	double * mScales; 		///< Scaling factors for the parameters
	pair<double, double> * mMinMax; ///< Minimum/maximum values
	vector<string> mParamNames; ///< Parameter names

	string mName; 			///< The name of this object, used in some serialization methods

public:
	CParameters(const CParameters & other);
	CParameters(unsigned int n_params);
	virtual ~CParameters();

protected:
	void CountFree(void);
	void CalculateScale(unsigned int param_num);

public:
	void GetFreeParams(double * params, unsigned int n_params, bool scale_params);
	vector<string> GetFreeParamNames();
	string GetName(void) { return mName; };
	int GetNFreeParams(void) { return mNFreeParams; };
	int GetNParams(void) { return mNParams; };
	double GetMax(unsigned int param_num);
	double GetMin(unsigned int param_num);
	vector< pair<double, double> > GetFreeMinMaxes();
	void GetParams(double * params, unsigned int n_params);
	double GetParam(unsigned int i);
	vector<string> GetParamNames();
	vector< pair<int, string> > GetParamIDsNames();
	string GetParamName(unsigned int param_num);

	bool IsFree(unsigned int param_num);

	virtual void Restore(Json::Value input);

	void SetAllFree(bool is_free);
	void SetFreeParams(double * in_params, unsigned int n_params, bool scale_params);
	void SetFree(unsigned int param_num, bool is_free);
	void SetMin(unsigned int param_num, double value);
	void SetMax(unsigned int param_num, double value);
	void SetName(string name) { mName = name; };
	void SetParam(unsigned int param_num, double value);
	virtual Json::Value Serialize();
};

#endif /* CPARAMETERS_H_ */
