/*
 * CGridSearch.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: bkloppen
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

#include "CGridSearch.h"

#include <tuple>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "CWorkerThread.h"
#include "CModelList.h"

using namespace std;

CGridSearch::CGridSearch()
{
	mMinimizerID = "gridsearch";
	mMinimizerName = "Gridsearch";
}

CGridSearch::~CGridSearch() {
	// TODO Auto-generated destructor stub
}

/// \brief Create a new CGridSearch object
CMinimizerPtr CGridSearch::Create()
{
	return CMinimizerPtr(new CGridSearch());
}

/// \brief Export results from this minimizer.
///
/// Overrides the base class method to copy the best-fit parameters into the
/// mParams buffer.
void CGridSearch::ExportResults()
{
	// Copy the best-fit parameters into the mParams buffer
	for(unsigned int i = 0; i < mNParams; i++)
		mParams[i] = mBestFit[i];

	// Call the base-class function:
	CMinimizerThread::ExportResults();
}

/// \brief A recurisve gridsearch function
///
/// Recursivly calls gridsearch until the final level is found. At this level
/// the data are simulated and written to mOutputFile. Data are flushed to disk
/// after a level %2 == 0 iteration completes.
void CGridSearch::GridSearch(unsigned int level)
{
	// If we just set the last parameter, get the chi2r
	if(level == mNParams)
	{
		// Get the chi2r for the data set
	    CModelListPtr model_list = mWorkerThread->GetModelList();
		model_list->SetFreeParameters(mParams, mNParams, false);
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		double chi2r = ComputeChi2r(mChis, mNParams);

		// Save to the file
		WriteRow(mParams, mNParams, chi2r, mOutputFile);

		// If this set of parameters fits better, replace the best-fit params.
		if(chi2r < mBestFit[mNParams])
		{
			for(int i = 0; i < mNParams; i++)
				mBestFit[i] = mParams[i];

			mBestFit[mNParams] = chi2r;
		}
	}
	else	// Otherwise set the current parameter value and then recursivly call the next level.
	{
		double step = mSteps[level];
		double min = mMinMax[level].first;
		double max = mMinMax[level].second + step;
		for(double value = min; value < max; value += step)
		{
			if(!mRun)
				break;

			mParams[level] = value;
			GridSearch(level + 1);
		}

		// If we are on an even level, flush the results to a file.
		if(level % 2 == 0)
			mOutputFile.flush();
	}
}

/// \brief Initialize the gridsearch minimizer.
void CGridSearch::Init(shared_ptr<CWorkerThread> worker_thread)
{
	CMinimizerThread::Init(worker_thread);

	mSteps.resize(mNParams);
}

/// \brief Run the gridsearch minimzer
void CGridSearch::run()
{
	// Get the min/max ranges for the parameters:
    CModelListPtr model_list = mWorkerThread->GetModelList();
    model_list->GetFreeParameters(mParams, mNParams, true);
	mMinMax = model_list->GetFreeParamMinMaxes();
	model_list->GetFreeParameterSteps(&mSteps[0], mSteps.size());

	// Verify that all of the steps are > 0
	for(double step: mSteps)
	{
		if(step <= 0)
			throw runtime_error("Step size for one parameter is zero. Please fix and try again!");
	}

	// Resize the best-fit parameter array to hold the best-fit parameters
	// and their chi2r (as the last element)
	mBestFit.resize(mNParams + 1);
	mBestFit[mNParams] = 1E6;	// Init the best-fit chi2r to some bogus value.

	// Open the statistics file for writing:
	stringstream filename;
	filename.str("");
	filename << mSaveDirectory << "/gridsearch.txt";
	mOutputFile.open(filename.str().c_str());
	mOutputFile.width(15);
	mOutputFile.precision(8);
	// write a somewhat descriptive header
	mOutputFile << "Param0, Param1, ..., ParamN, chi2r" << endl;

	// run the minimizer
	mIsRunning = true;
	GridSearch(0);
	mIsRunning = false;

	mOutputFile.close();

	// Export the results.
	ExportResults();
}
