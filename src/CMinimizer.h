/*
 * CMinimizer.h
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 *
 *  A generic wrapper class for minimization routines.  This class implements the
 *  basic thread synchronization tasks that are required to interop with SIMTOI.
 *
 *  TODO: I'm not fully convinced the scaling of the parameters should be here, but some
 *  minimization programs, like MultiNest, expect the parameters to be on a uniform hypercube
 *  so for now this is how it will be implemented.  Eventually, perhaps, the models could do
 *  the scaling themselves?
 */

#ifndef CMINIMIZER_H_
#define CMINIMIZER_H_

#include <string>
#include <cstdio>
#include "boost/function.hpp"
#include "boost/bind.hpp"

using namespace std;

class CCL_GLThread;

class CMinimizer
{
protected:
	CCL_GLThread * mCLThread;
	float * mParams;

public:
	CMinimizer(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer();

public:
	virtual void Init();
	virtual int run() = 0;
};

#endif /* CMINIMIZER_H_ */
