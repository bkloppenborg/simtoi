/*
 * CMinimizer_levmar.h
 *
 *  Created on: Feb 13, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_LEVMAR_H_
#define CMINIMIZER_LEVMAR_H_

#include "CMinimizer.h"

class CMinimizer_levmar: public CMinimizer
{
protected:
	float * mResiduals;

public:
	CMinimizer_levmar(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_levmar();

	static void ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc);

	string GetExitString(int exit_num);

	void Init();

	void printresult(double * x, int n_pars, int n_data, vector<string> names, double * info);

	int run();
};

#endif /* CMINIMIZER_LEVMAR_H_ */
