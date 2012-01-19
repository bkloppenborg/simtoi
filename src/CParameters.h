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

public:
	CParameters(int n_params);
	virtual ~CParameters();

protected:
	void CountFree(void);

public:
	void GetParams(float * params, int n_params);
	vector<string> GetParamNames();
	string GetParamName(int i);
	int GetNFreeParams(void) { return mNFreeParams; };

	void SetFree(int param_num, bool is_free);
	void SetParams(float * in_params, int n_params);
};

#endif /* CPARAMETERS_H_ */
