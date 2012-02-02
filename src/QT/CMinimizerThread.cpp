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

	mMinimizer->run();
}

void CMinimizerThread::SetMinimizer(CMinimizer * minimizer)
{
	delete mMinimizer;

	mMinimizer = minimizer;
}
