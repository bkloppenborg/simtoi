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

CMinimizerPtr CGridSearch::Create()
{
	return CMinimizerPtr(new CGridSearch());
}

void CGridSearch::ExportResults()
{
	// Copy the best-fit parameters into the mParams buffer
	for(unsigned int i = 0; i < mNParams; i++)
		mParams[i] = mBestFit[i];

	// Call the base-class function:
	CMinimizerThread::ExportResults();
}

void CGridSearch::GridSearch(unsigned int level)
{
	// If we just set the last parameter, get the chi2r
	if(level == mNParams)
	{
	    CModelListPtr model_list = mWorkerThread->GetModelList();
		model_list->SetFreeParameters(mParams, mNParams, false);
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		double chi2r = ComputeChi2r(mChis, mNParams);

		WriteRow(mParams, mNParams, chi2r, mOutputFile);

		// If this set of parameters fits better, replace the best-fit params.
		if(chi2r < mBestFit[mNParams])
		{
			for(int i = 0; i < mNParams; i++)
				mBestFit[i] = mParams[i];

			mBestFit[mNParams] = chi2r;
		}
	}
	else
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

void CGridSearch::Init(shared_ptr<CWorkerThread> worker_thread)
{
	CMinimizerThread::Init(worker_thread);

	mSteps.resize(mNParams);
}

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

	stringstream filename;

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveFolder << "/gridsearch.txt";
	mOutputFile.open(filename.str().c_str());
	mOutputFile.width(15);
	mOutputFile.precision(8);

	mIsRunning = true;
	GridSearch(0);
	mIsRunning = false;

	mOutputFile.close();

	// Export the results.  Note, mParams is set to the maximum value, not the minimum.
	ExportResults();
}

void CGridSearch::WriteRow(double * data, unsigned int size, double chi2r, ofstream & output)
{
	for(int i = 0; i < size; i++)
		output << data[i] << ", ";

	output << chi2r << endl;
}
