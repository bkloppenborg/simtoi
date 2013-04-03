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

/// \brief Overwride the CMinimizerThread::ExportResult function to do nothing.
///
/// The CLevmar::run() function calls CMinimizerThread::ExportResult whenever a minimization
///	run completes. This writes out files which we don't need when bootstrapping. So we overwride
/// the CMinimizerThread::ExportResult function here with one that does nothing.
void CBootstrap_Levmar::ExportResults()
{
	// do nothing
}

void CBootstrap_Levmar::Init(shared_ptr<CWorkerThread> worker_thread)
{
	// call the base class initialization routine
	CMinimizerThread::Init(worker_thread);

	// Create an instance of CLevmar and initialize it
	mLevmar = CLevmar::Create();
	mLevmar->Init(this->mWorkerThread);
}

/// \brief Runs the levmar-based bootstrapping minimizer.
void CBootstrap_Levmar::run()
{
	// Open the statistics file for writing:
	stringstream filename;
	filename.str("");
	filename << mSaveFolder << "/bootstrap_levmar.txt";
	mOutputFile.open(filename.str().c_str());
	mOutputFile.width(15);
	mOutputFile.precision(8);
	// write a somewhat descriptive header
	mOutputFile << "Param0, Param1, ..., ParamN, chi2r" << endl;

	// init local storage
	double chi2r_ave = 0;
	double tmp = 0;
	int exit_value = 0;

	// The maximum chi2r that will be accepted. Iterations exceeding this value will be repeated.
	float chi2_threshold = 10;
	int chi2r_exceeded = 0;
	int nData = 0;

	// Setup the random number generator:
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	uniform_real_distribution<double> distribution (0.0,1.0);

	// Set the parameters (note, they are not scaled to unit magnitude)
	CModelListPtr model_list = mWorkerThread->GetModelList();

	vector< pair<double, double> > min_max = model_list->GetFreeParamMinMaxes();

	for(int iteration = 0; iteration < mIterations; iteration++)
	{
		if(!mRun)
			break;

		// Randomize the starting position of the minimizer
		for(int i = 0; i < mLevmar->mNParams; i++)
			mLevmar->mParams[i] = distribution(generator);

		// Set the starting position.  Note these values are [0...1] and need to be scaled.
		model_list->SetFreeParameters(mLevmar->mParams, mLevmar->mNParams, true);

		// Print a status message:
		cout << endl << "Starting iteration " << iteration + 1 << endl;

		// run the minimizer. The best-fit parameters are stored in mParams upon completion
		mLevmar->run();

		// Set the best-fit parameters, then compute the average reduced chi2 per data set
		model_list->SetFreeParameters(mLevmar->mParams, mNParams, true);
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		chi2r_ave = ComputeChi2r(mChis, mNParams);

		// If the average reduced chi2 is too high automatically redo the bootstrap
		if(chi2r_ave > chi2_threshold)
		{
			cerr << " Average Chi2r = " << chi2r_ave << " exceeds chi2_threshold = " << chi2_threshold << " repeating iteration " << iteration << "." << endl;
			cout << " Average Chi2r = " << chi2r_ave << " exceeds chi2_threshold = " << chi2_threshold << " repeating iteration " << iteration << "." << endl;
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

}

/// \brief Stops the minimizer.
void CBootstrap_Levmar::stop()
{
	mRun = false;
	if(mLevmar)
		mLevmar->mRun = false;
}
