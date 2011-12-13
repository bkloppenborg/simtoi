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

#include <boost/thread.hpp>

enum MinimizationType{
	Chi2,
	LogLike
};

class CModelList;
class CLibOI;

class CMinimizer
{
protected:
	MinimizationType mType;

public:
	CMinimizer(MinimizationType type, CModelList * models, CLibOI * ocl);
	virtual ~CMinimizer();

public:
	MinimizationType GetMinType() { return mType; };



};

#endif /* CMINIMIZER_H_ */
