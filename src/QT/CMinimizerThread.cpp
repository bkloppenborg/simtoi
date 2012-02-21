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

	// Get this thing to run as often as possible.
	//Priority = QThread::TimeCriticalPriority;
}

CMinimizerThread::~CMinimizerThread()
{
	// Free memory (if it's still allocated)
	delete mMinimizer;
}

void CMinimizerThread::run()
{
	if(!mMinimizer)
		return;

	// Call init before running the minimzer to setup memory.
	mMinimizer->Init();
	mMinimizer->run();
	exit();
}

/// Sets the minimizer, freeing the old minimizer if needed.
void CMinimizerThread::SetMinimizer(CMinimizer * minimizer)
{
	if(mMinimizer != NULL && !mMinimizer->IsRunning())
		delete mMinimizer;

	mMinimizer = minimizer;
}

void CMinimizerThread::stop()
{
	mMinimizer->Stop();
}
