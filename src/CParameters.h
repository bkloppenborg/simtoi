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
	float * mParams;
	int mNFreeParams;
	bool * mFreeParams;
	float * mScales;
	pair<float, float> * mMinMax;
	vector<string> mParamNames;

	string mName;

public:
	CParameters(int n_params);
	virtual ~CParameters();

protected:
	void CountFree(void);
	void CalculateScale(int param_num);

public:
	void GetFreeParams(float * params, int n_param, bool scale_params);
	vector<string> GetFreeParamNames();
	string GetName(void) { return mName; };
	int GetNFreeParams(void) { return mNFreeParams; };
	int GetNParams(void) { return mNParams; };
	float GetMax(int param_num);
	float GetMin(int param_num);
	vector< pair<float, float> > GetFreeMinMaxes();
	void GetParams(float * params, unsigned int n_params);
	float GetParam(int i);
	vector<string> GetParamNames();
	vector< pair<int, string> > GetParamIDsNames();
	string GetParamName(unsigned int param_num);

	bool IsFree(int param_num);

	virtual void Restore(Json::Value input);

	void SetAllFree(bool is_free);
	void SetFreeParams(float * in_params, int n_params, bool scale_params);
	void SetFree(int param_num, bool is_free);
	void SetMin(int param_num, float value);
	void SetMax(int param_num, float value);
	void SetName(string name) { mName = name; };
	void SetParam(int n_param, float value);
	virtual Json::Value Serialize();
};

#endif /* CPARAMETERS_H_ */
