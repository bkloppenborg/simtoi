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
//#include <boost/thread/thread.hpp>

using namespace std;

class CSIMTOI;

class CMinimizer
{
protected:
	//boost::thread * mThread;
	CSIMTOI * mSIMTOI;	// A pointer to SIMTOI methods.  Not allocated here so don't deallocate.

public:
	CMinimizer(CSIMTOI * simtoi);
	virtual ~CMinimizer();

public:
	static CMinimizer * GetMinimizer(string name, CSIMTOI * simtoi);

	virtual int Init() {};

	void Run();

	void Stop();

	virtual int ThreadFunc() = 0;
};

#endif /* CMINIMIZER_H_ */
