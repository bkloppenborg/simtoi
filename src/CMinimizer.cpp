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

using namespace std;

CMinimizer::CMinimizer()
{
	mParams = NULL;
	mNParams = 0;
	mRun = false;
	mIsRunning = false;
	mSaveFileBasename = "/tmp/model";
	mMinimizerName = "";
	mMinimizerID = "";
}

CMinimizer::~CMinimizer()
{
	// Cleanly stop the thread
	stop();

	// Clear up memory
	delete[] mParams;
}

/// Saves the V2, and T3 data along with best-fit model simulated data
/// for each epoch.
void CMinimizer::ExportResults(double * params, int n_params, bool no_setparams)
{
	stringstream filename;
	ofstream outfile;

	vector<string> names = mCLThread->GetFreeParamNames();

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveFileBasename << "_param_names.txt";
	outfile.open(filename.str().c_str());
	outfile.width(15);
	outfile.precision(8);
	outfile << "# Parameter names in a column." << endl;
	outfile << "# Param0, ..., ParamN" << endl;
	WriteRow(names, outfile);
	outfile.close();

	if(!no_setparams)
	{
		n_params = min(n_params, int(mNParams));

	    //printf("Generating Plots using best-fit parameters:\n");
		for(int i = 0; i < n_params; i++)
			printf("%i: %e \n", i, params[i]);

		mCLThread->SetFreeParameters(params, n_params, true);
	}

	mCLThread->ExportResults(mSaveFileBasename);
}

/// Returns the best-fit parameters.
void CMinimizer::GetResults(double * results, int n_params)
{
	// Copy the values
	for(int i = 0; i < mNParams && i < n_params; i++)
		results[i] = mParams[i];
}

void CMinimizer::Init(shared_ptr<CCL_GLThread> cl_gl_thread)
{
	mCLThread = cl_gl_thread;
	mNParams = mCLThread->GetNFreeParameters();
	mParams = new double[mNParams];
	mRun = true;
}

void CMinimizer::SetSaveFileBasename(string filename)
{
	// Only permit non-empty save file names:
	if(filename.size() > 0)
		mSaveFileBasename = filename;
}

void CMinimizer::start(CMinimizerPtr minimizer)
{
	minimizer->run();
}

void CMinimizer::stop()
{
	mRun = false;
}
