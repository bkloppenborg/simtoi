/*
 * CMinimizerThread.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizerThread.h"
#include "CMinimizer.h"

CMinimizerThread::CMinimizerThread()
{
	mMinimizer = NULL;
}

CMinimizerThread::~CMinimizerThread()
{
	// TODO Auto-generated destructor stub
}

void CMinimizerThread::run()
{
	if(!mMinimizer)
		return;

	// Call init before running the minimzer to setup memory.
	mMinimizer->Init();
	mMinimizer->run();

	emit ParametersChanged();
}

void CMinimizerThread::SetMinimizer(CMinimizer * minimizer)
{
	delete mMinimizer;

	mMinimizer = minimizer;
}
