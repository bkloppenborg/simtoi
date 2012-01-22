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
	float * mScales_min;
	vector<string> mParamNames;

	string mName;

public:
	CParameters(int n_params);
	virtual ~CParameters();

protected:
	void CountFree(void);

public:
	string GetName(void) { return mName; };
	int GetNFreeParams(void) { return mNFreeParams; };
	int GetNParams(void) { return mNParams; };
	void GetParams(float * params, int n_params);
	float GetParam(int i);
	vector<string> GetParamNames();
	vector< pair<int, string> > GetParamIDsNames();
	string GetParamName(int param_num);

	bool IsFree(int param_num);

	void SetAllFree(bool is_free);
	void SetFree(int param_num, bool is_free);
	void SetName(string name) { mName = name; };
	void SetParam(int n_param, float value);
	void SetParams(float * in_params, int n_params);
};

#endif /* CPARAMETERS_H_ */
