/*
 * CBootstrap_Levmar.cpp
 *
 *  Created on: Mar 29, 2013
 *      Author: bkloppenborg
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
	mMinimizerID = "bootstrap_levmar";
	mMinimizerName = "Bootstrap - Levmar";

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

/// \brief Overwride the CMinimizerThread::ExportResult
///
/// The CLevmar::run() function calls CMinimizerThread::ExportResult whenever a minimization
///	run completes. This writes out files which we don't need when bootstrapping. Here we
/// override that functionality and only write out the names of the best-fit parameters.
void CBootstrap_Levmar::ExportResults()
{
	ofstream outfile;

	// Get a pointer to the model list;
    CModelListPtr model_list = mWorkerThread->GetModelList();
	vector<string> names = model_list->GetFreeParamNames();

	OpenStatisticsFile(outfile);
	WriteHeader(names, outfile);

	outfile.flush();
	outfile.close();
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

	for(int iteration = 0; iteration < mIterations; iteration++)
	{
		if(!mRun)
			break;

		// Set the starting position.  Note these values are [0...1] and need to be scaled.
		model_list->SetFreeParameters(&nominal_params[0], mLevmar->mNParams, true);

		// Print a status message:
		cout << endl << "Starting iteration " << iteration + 1 << endl;

		// run the minimizer. The best-fit parameters are stored in mParams upon completion
		mLevmar->run(&CLevmar::ErrorFunc);

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

		// Get the next bootstrapped data set and repeat.
		mWorkerThread->BootstrapNext(mMaxBootstrapFailures);
	}

	ExportResults();

}

/// \brief Stops the minimizer.
void CBootstrap_Levmar::stop()
{
	mRun = false;
	if(mLevmar)
		mLevmar->mRun = false;
}
