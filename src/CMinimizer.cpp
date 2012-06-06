/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
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

#include "CMinimizer.h"
#include "CCL_GLThread.h"
#include "CMinimizer_Benchmark.h"
#include "CMinimizer_MultiNest.h"
#include "CMinimizer_levmar.h"
#include "CMinimizer_Test.h"

CMinimizer::CMinimizer(CCL_GLThread * cl_gl_thread)
{
	mCLThread = cl_gl_thread;
	mParams = NULL;
	mNParams = 0;
	mType = NONE;
	mRun = true;
	mIsRunning = true;
	mResultsBaseFilename = "/tmp/min_output";
}

CMinimizer::~CMinimizer()
{
	delete[] mParams;
}

/// Saves the V2, and T3 data along with best-fit model simulated data
/// for each epoch.
void CMinimizer::ExportResults(double * params, int n_params, bool no_setparams)
{
	if(!no_setparams)
	{
		n_params = min(n_params, int(mNParams));

	    printf("Generating Plots using best-fit parameters:\n");
		for(int i = 0; i < n_params; i++)
			printf("%i: %e \n", i, params[i]);

		mCLThread->SetFreeParameters(params, n_params, true);
	}

	mCLThread->ExportResults(mResultsBaseFilename);
}

CMinimizer * CMinimizer::GetMinimizer(CMinimizer::MinimizerTypes type, CCL_GLThread * cl_gl_thread)
{
	CMinimizer * tmp;
	switch(type)
	{
	case MULTINEST:
		tmp = new CMinimizer_MultiNest(cl_gl_thread);
		break;

	case LEVMAR:
		tmp = new CMinimizer_levmar(cl_gl_thread);
		break;

	case TEST:
		tmp = new CMinimizer_Test(cl_gl_thread);
		break;

	default:
	case BENCHMARK:
		tmp = new CMinimizer_Benchmark(cl_gl_thread);
		break;
	}

	return tmp;
}

vector< pair<CMinimizer::MinimizerTypes, string> > CMinimizer::GetTypes(void)
{
	vector< pair<CMinimizer::MinimizerTypes, string> > tmp;
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::BENCHMARK, "Benchmark"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::MULTINEST, "MultiNest"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::LEVMAR, "Levmar"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::TEST, "Tests"));
	return tmp;
}

void CMinimizer::Init()
{
	mParams = new double[mCLThread->GetNFreeParameters()];
}

bool CMinimizer::IsRunning()
{
	return mIsRunning;
}

/// Tells the thread to gracefully exit.
/// The event loop of the minimization thread must mRun for this routine to work correctly.
void CMinimizer::Stop()
{
	mRun = false;
}
