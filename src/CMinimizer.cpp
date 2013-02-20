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
#include "CWorkerThread.h"

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

void CMinimizer::ComputeChi(valarray<double> & residuals, const valarray<double> & uncertainties, valarray<double> & results)
{
	// Valarray operations happen
	results = residuals / uncertainties;
}

double CMinimizer::ComputeChi2r(valarray<double> & residuals, const valarray<double> & uncertainties, unsigned int n_params)
{
	// Square the numerator, divide by the uncertainties
	residuals *= residuals;
	residuals /= uncertainties;

	// Now compute the sum dividied by (n_data - n_params - 1)
	return residuals.sum() / (residuals.size() - n_params - 1);
}

/// \brief Saves the V2, and T3 data along with best-fit model simulated data
/// for each epoch.
void CMinimizer::ExportResults(double * params, int n_params, bool no_setparams)
{
//	stringstream filename;
//	ofstream outfile;
//
//	vector<string> names = mWorkerThread->GetFreeParamNames();
//
//	// Open the statistics file for writing:
//	filename.str("");
//	filename << mSaveFileBasename << "_param_names.txt";
//	outfile.open(filename.str().c_str());
//	outfile.width(15);
//	outfile.precision(8);
//	outfile << "# Parameter names in a column." << endl;
//	outfile << "# Param0, ..., ParamN" << endl;
//	WriteRow(names, outfile);
//	outfile.close();
//
//	if(!no_setparams)
//	{
//		n_params = min(n_params, int(mNParams));
//
//	    //printf("Generating Plots using best-fit parameters:\n");
//		for(int i = 0; i < n_params; i++)
//			printf("%i: %e \n", i, params[i]);
//
//		mWorkerThread->SetFreeParameters(params, n_params, true);
//	}
//
//	mWorkerThread->ExportResults(mSaveFileBasename);
}

/// \brief Returns the unique ID assigned to this minimizer.
string CMinimizer::GetID()
{
	return mMinimizerID;
}

/// \brief Returns the best-fit parameters after minimization has completed.
///
/// Once the minimization is complete the minimizer is *required* to set the `mParams`
/// variable to the best-fit minimization parameters. This function can be used
/// to copy the first `n_params` values into the `results` buffer.
///
/// \param results A pre-allocated buffer of size `n_params` to which results are written.
/// \param n_params The size of the `results` buffer.
void CMinimizer::GetResults(double * results, int n_params)
{
	// Copy the values
	for(int i = 0; i < mNParams && i < n_params; i++)
		results[i] = mParams[i];
}

/// \brief Initialization function.
///
/// \param worker_thread A shared pointer to the worker thread
void CMinimizer::Init(shared_ptr<CWorkerThread> worker_thread)
{
	mWorkerThread = worker_thread;
//	mNParams = mWorkerThread->GetNFreeParameters();
	mNParams = 1;
	mParams = new double[mNParams];
	mRun = true;
}

/// \brief Changes the default filename which the minimizer uses for savefiles.
///
/// Some minimization engines may write out intermediate files during the
/// minimization process. This function permits you to set an absolute path
/// and filename prefix which is used by the minimizers.
///
/// \param filename An absolute path prefix for minimizer files.
void CMinimizer::SetSaveFileBasename(string filename)
{
	// Only permit non-empty save file names:
	if(filename.size() > 0)
		mSaveFileBasename = filename;
}

/// \brief Starts the minimizer.
void CMinimizer::start(CMinimizerPtr minimizer)
{
	minimizer->run();
}

/// \brief Stops the minimizer.
void CMinimizer::stop()
{
	mRun = false;
}
