/*
 * CBenchmark.cpp
 *
 *  Created on: Feb 10, 2012
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

#include "CBenchmark.h"
#include <sys/timeb.h>
#include "CCL_GLThread.h"
#include "misc.h"

CBenchmark::CBenchmark()
{
	mMinimizerID = "benchmark";
	mMinimizerName = "Benchmark";
}

CBenchmark::~CBenchmark()
{
	// TODO Auto-generated destructor stub
}

CMinimizerPtr CBenchmark::Create()
{
	return CMinimizerPtr(new CBenchmark());
}

int CBenchmark::GetMilliCount()
{
	// Something like GetTickCount but portable
	// It rolls over every ~ 12.1 days (0x100000/24/60/60)
	// Use GetMilliSpan to correct for rollover
	timeb tb;
	ftime( &tb );
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int CBenchmark::GetMilliSpan( int nTimeStart )
{
	int nSpan = GetMilliCount() - nTimeStart;
	if ( nSpan < 0 )
		nSpan += 0x100000 * 1000;
	return nSpan;
}

/// Runs the benchmark minimizer
/// This simply runs n_iterations iterations as fast as possible, timing the result
/// and reporting it to the user.
int CBenchmark::run()
{
	mIsRunning = true;
	// only the first loaded data set is used.
	float chi2r = 0;
	double chi_cpu = 0;
	int n_iterations = 1000;
	double time = 0;
	int nData = mCLThread->GetNDataAllocated(0);
	float output[nData];

	int start = GetMilliCount();

	for(int i = 0; i < n_iterations && mRun; i++)
	{
		mCLThread->SetTime(mCLThread->GetDataAveJD(0));
		mCLThread->EnqueueOperation(GLT_RenderModels);
		chi2r = mCLThread->GetChi2(0) / (nData - mNParams - 1);

		if(i % 100 == 0)
			printf("Iteration %i Chi2r: %f\n", i, chi2r);
	}

	// Calculate the time, print out a nice message.
	time = double(GetMilliSpan(start)) / 1000;
	printf("Benchmark Test completed!\n %i iterations in %f seconds, throughput %f iterations/sec.\n", n_iterations, time, n_iterations/time);

	mIsRunning = false;
	return 0;
}

