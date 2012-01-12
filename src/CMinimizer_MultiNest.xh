/*
 * CMinimizer_MultiNest.h
 *
 *  Created on: Dec 13, 2011
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_MULTINEST_H_
#define CMINIMIZER_MULTINEST_H_

#include "CMinimizer.h"
#include <string>

using namespace std;

class CMinimizer_MultiNest: public CMinimizer
{
private:
	string mOutput;

public:
	CMinimizer_MultiNest(CSIMTOI * simtoi);
	virtual ~CMinimizer_MultiNest();

	int ThreadFunc();
	int Init() { return 0; };

	// Functions for multinest:
	static void log_likelihood(double *Cube, int *ndim, int *npars, double *lnew);
	static void dumper(int *nSamples, int *nlive, int *nPar, double **physLive, double **posterior, double *paramConstr, double *maxLogLike, double *logZ, double *logZerr);
};

#endif /* CMINIMIZER_MULTINEST_H_ */
