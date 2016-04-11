 /*
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
 *
 *  Copyright (c) 2013 Brian Kloppenborg
 */

#include "CBootstrap_Levmar.h"

#include <random>
#include <chrono>
#include <stdexcept>

#include "CLevmar.h"
#include "CWorkerThread.h"
#include "CModelList.h"

using namespace std;

CBootstrap_Levmar::CBootstrap_Levmar()
{
	mID = "bootstrap_levmar";
	mName = "Bootstrap using Levmar (Levenberg-Marquart) - local";

	mBootstrapFailures = 0;
	mMaxBootstrapFailures = 20;
	mIterations = 10000;
}

CBootstrap_Levmar::~CBootstrap_Levmar()
{

}

CMinimizerPtr CBootstrap_Levmar::Create()
{
	return CMinimizerPtr(new CBootstrap_Levmar());
}

void CBootstrap_Levmar::Init(shared_ptr<CWorkerThread> worker_thread)
{
	// call the base class initialization routine
	CMinimizerThread::Init(worker_thread);

	// Create an instance of CLevmar and initialize it. Notice we still wrap in a shared_ptr
	mLevmar = shared_ptr<CLevmar>(new CLevmar());
	mLevmar->Init(this->mWorkerThread);
}

/// \brief Runs the levmar-based bootstrapping minimizer.
void CBootstrap_Levmar::run()
{
	// Open the statistics file for writing:
	stringstream filename;
	filename.str("");
	filename << mSaveDirectory << "/bootstrap_levmar.txt";
	mOutputFile.open(filename.str().c_str());
	mOutputFile.width(15);
	mOutputFile.precision(8);
	// write a somewhat descriptive header
	mOutputFile << "# Param0, Param1, ..., ParamN, chi2r" << endl;

	// init local storage
	double chi2r_ave = 0;
	double tmp = 0;
	int exit_value = 0;

	// The maximum chi2r that will be accepted. Iterations exceeding this value will be repeated.
	float chi2_threshold = 15;
	int chi2r_exceeded = 0;
	int nData = 0;

	// Set the parameters (note, they are not scaled to unit magnitude)
	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Look up the nominal parameters from the model.
	valarray<double> nominal_params(mLevmar->mNParams);
	model_list->GetFreeParameters(&nominal_params[0], mLevmar->mNParams, false);

	// Run one iteration with the original data and best-fit values as specified
	// in the model file.
	model_list->SetFreeParameters(&nominal_params[0], mLevmar->mNParams, false);
	mWorkerThread->GetChi(&mChis[0], mChis.size());
	chi2r_ave = ComputeChi2r(mChis, mNParams);
	WriteRow(&nominal_params[0], mLevmar->mNParams, chi2r_ave, mOutputFile);

	// Now run the bootstraps.
	for(int iteration = 0; iteration < mIterations; iteration++)
	{
		if(!mRun)
			break;

		// Get a bootstrapped data set
		mWorkerThread->BootstrapNext(mMaxBootstrapFailures);

		// Set the starting position.
		model_list->SetFreeParameters(&nominal_params[0], mLevmar->mNParams, false);

		// Print a status message:
		cout << endl << "Starting iteration " << iteration + 1 << endl;

		// run the minimizer. The best-fit parameters are stored in mParams upon completion
		try
		{
			mLevmar->run(&CLevmar::ErrorFunc);
		}
		catch(range_error & e)
		{
			cerr << " Warning: A parameter range error was detected: " << e.what() << endl;
			cerr << " This iteration will be repeated." << endl;
			iteration--;
			continue;
		}

		// Set the best-fit parameters, then compute the average reduced chi2 per data set
		model_list->SetFreeParameters(mLevmar->mParams, mNParams, false);
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		chi2r_ave = ComputeChi2r(mChis, mNParams);

		// If the average reduced chi2 is too high automatically redo the bootstrap
		if(chi2r_ave > chi2_threshold)
		{
			cerr << " Average Chi2r = " << chi2r_ave << " exceeds chi2_threshold = " << chi2_threshold << " repeating iteration " << iteration + 1 << "." << endl;
			cout << " Average Chi2r = " << chi2r_ave << " exceeds chi2_threshold = " << chi2_threshold << " repeating iteration " << iteration + 1 << "." << endl;
			chi2r_exceeded += 1;
			iteration--;

			if(chi2r_exceeded < mMaxBootstrapFailures)
				continue;
			else
				throw runtime_error("Maximum chi2r trials exceeded.");
		}
		chi2r_exceeded = 0;

		// Write the results to a file:
		// Save to the file
		WriteRow(mLevmar->mParams, mLevmar->mNParams, chi2r_ave, mOutputFile);
	}

	// Copy the nominal values back to the best-fit values

	for(unsigned int i = 0; i < mNParams; i++)
		mParams[i] = nominal_params[i];

	ExportResults();

}

/// \brief Stops the minimizer.
void CBootstrap_Levmar::stop()
{
	mRun = false;
	if(mLevmar)
		mLevmar->mRun = false;
}
