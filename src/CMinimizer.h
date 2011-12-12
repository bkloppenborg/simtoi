/*
 * CMinimizer.h
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 *
 *  A generic wrapper class for minimization routines.  This class implements the
 *  basic thread synchronization tasks that are required to interop with SIMTOI.
 */

#ifndef CMINIMIZER_H_
#define CMINIMIZER_H_

#include <boost/thread.hpp>

enum MinimizationType{
	Chi2,
	LogLike
};

class CMinimizer
{
	MinimizationType mType;
	float * mParamBounds;
	int mNParams;

public:
	CMinimizer(MinimizationType type);
	virtual ~CMinimizer();

	// The minimization function to be called from SIMTOI.
	virtual void Minimize(float err, float * params, int n_params) = 0;

	void SetParamBounds(float * bounds, int n_params);

	MinimizationType GetMinType() { return mType; };


};

#endif /* CMINIMIZER_H_ */
