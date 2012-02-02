/*
 * CMinimizer_mpfit.h
 *
 *  Created on: Feb 1, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_MPFIT_H_
#define CMINIMIZER_MPFIT_H_

#include "CMinimizer.h"

class CMinimizer_mpfit: public CMinimizer
{
	float * mResiduals;

public:
	CMinimizer_mpfit(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_mpfit();

	static int ErrorFunc(int nData, int nParams, double * params, double * deviates, double ** derivs, void * misc);

	void Init();

	int run();
};

#endif /* CMINIMIZER_MPFIT_H_ */
