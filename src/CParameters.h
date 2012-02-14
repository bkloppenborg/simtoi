/*
 * CParameters.h
 *
 *  Created on: Jan 18, 2012
 *      Author: bkloppenborg
 *
 *  Basic class for storing, setting, and getting parameters.
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
	string GetName(void) { return mName; };
	int GetNFreeParams(void) { return mNFreeParams; };
	int GetNParams(void) { return mNParams; };
	double GetMax(int param_num);
	double GetMin(int param_num);
	vector< pair<double, double> > GetFreeMinMaxes();
	void GetParams(double * params, unsigned int n_params);
	double GetParam(int i);
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
