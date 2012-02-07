/*
 * CMinimizer_mpfit.h
 *
 *  Created on: Feb 1, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_MPFIT_H_
#define CMINIMIZER_MPFIT_H_

#include "CMinimizer.h"
#include "mpfit.h"

class CMinimizer_mpfit: public CMinimizer
{
	float * mResiduals;

public:
	CMinimizer_mpfit(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_mpfit();

	static int ErrorFunc(int nData, int nParams, double * params, double * deviates, double ** derivs, void * misc);

	void Init();

	void printresult(double * x, mp_result * result, vector<string> names, vector< pair<float, float> > min_max);

	int run();
};

#endif /* CMINIMIZER_MPFIT_H_ */
