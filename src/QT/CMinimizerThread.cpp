/*
 * CMinimizerThread.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
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
	// If the minimizer exists and it is running, do nothing.
	if(mMinimizer != NULL && mMinimizer->IsRunning())
		return;

	// Free memory and set the new minimizer
	delete mMinimizer;
	mMinimizer = minimizer;
}

void CMinimizerThread::stop()
{
	if(mMinimizer != NULL)
		mMinimizer->Stop();
}
