/*
 * CMinimizer_MultiNest.h
 *
 *  Created on: Jan 26, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_MULTINEST_H_
#define CMINIMIZER_MULTINEST_H_

#include "CMinimizer.h"
#include "multinest.h"

class CMinimizer_MultiNest: public CMinimizer
{

public:
	CMinimizer_MultiNest(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_MultiNest();

	static void log_likelihood(double * Cube, int & ndim, int & npars, double & lnew, void * misc);
	static void dumper(int & nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double **paramConstr, double &maxLogLike, double &logZ, double &logZerr, void * misc);

	int run();

};

#endif /* CMINIMIZER_MULTINEST_H_ */
