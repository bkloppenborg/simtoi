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

class CParameters
{
protected:
	// Varaibles for parameter values
	int mNParams;
	double * mParams;
	int mNFreeParams;
	bool * mFreeParams;
	double * mScales;
	pair<double, double> * mMinMax;
	vector<string> mParamNames;

	string mName;

public:
	CParameters(int n_params);
	virtual ~CParameters();

protected:
	void CountFree(void);
	void CalculateScale(int param_num);

public:
	void GetFreeParams(double * params, int n_param, bool scale_params);
	vector<string> GetFreeParamNames();
	double GetFreePriorProd();
	string GetName(void) { return mName; };
	int GetNFreeParams(void) { return mNFreeParams; };
	int GetNParams(void) { return mNParams; };
	double GetMax(int param_num);
	double GetMin(int param_num);
	vector< pair<double, double> > GetFreeMinMaxes();
	void GetParams(double * params, unsigned int n_params);
	double GetParam(int i);
	double GetPrior(int i);
	vector<string> GetParamNames();
	vector< pair<int, string> > GetParamIDsNames();
	string GetParamName(unsigned int param_num);

	bool IsFree(int param_num);

	virtual void Restore(Json::Value input);

	void SetAllFree(bool is_free);
	void SetFreeParams(double * in_params, int n_params, bool scale_params);
	void SetFree(int param_num, bool is_free);
	void SetMin(int param_num, double value);
	void SetMax(int param_num, double value);
	void SetName(string name) { mName = name; };
	void SetParam(int n_param, double value);
	virtual Json::Value Serialize();
};

#endif /* CPARAMETERS_H_ */
