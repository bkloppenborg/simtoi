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
#include "CWorkerThread.h"
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

/// \brief Exports results from this minimzer
///
/// The benchmark minimizer should not export anything so we simply override
/// the base class function with one that does nothing.
void CBenchmark::ExportResults()
{
	// do nothing.
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
void CBenchmark::run()
{
	// setup locals
	mIsRunning = true;
	int n_iterations = 1000;
	double chi2r = 0;
	double time = 0;

	int start = GetMilliCount();

	for(int i = 0; i < n_iterations && mRun; i++)
	{
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		chi2r = ComputeChi2r(mChis, mNParams);

		if(i % 100 == 0)
			printf("Iteration %i Chi2r: %f\n", i, chi2r);
	}

	// Calculate the time, print out a nice message.
	time = double(GetMilliSpan(start)) / 1000;
	cout << "Benchmark Test completed!" << endl;
	cout << "Completed " << n_iterations << " iterations in " << time << " seconds." << endl;
	cout << "Throughput: " << n_iterations/time << " iterations/second." << endl;

	mIsRunning = false;
}

