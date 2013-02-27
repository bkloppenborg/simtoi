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
	// Call the base-class export function:
	CMinimizerThread::ExportResults();

	stringstream filename;
	ofstream outfile;
	double tmp[3];

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveFolder << "/gridsearch.txt";
	outfile.open(filename.str().c_str());
	outfile.width(15);
	outfile.precision(8);
	outfile << "# Param1 Param2 Chi2" << endl;

	// write the data to the file
	for(int i = 0; i < mResults.size(); i++)
		outfile << get<0>(mResults[i]) << " " << get<1>(mResults[i]) << " " << get<2>(mResults[i]) << endl;

	outfile.close();
}

void CGridSearch::GridSearch(unsigned int level)
{
	if(level = mNParams)
	{
	    CModelListPtr model_list = mWorkerThread->GetModelList();
		model_list->SetFreeParameters(mParams, mNParams, false);
		mWorkerThread->GetChi(&mChis[0], mChis.size());
		double chi2r = ComputeChi2r(mChis, mNParams);

		WriteRow(mParams, mNParams, mOutputFile);


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
	}
}

void CGridSearch::run()
{
	// Get the min/max ranges for the parameters:
    model_list->GetFreeParameters(mParams, mNParams, true);
	mMinMax = model_list->GetFreeParamMinMaxes();
	mSteps = model_list->GetFreeParamSteps();

	stringstream filename;

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveFolder << "/gridsearch.txt";
	mOutputFile.open(filename.str().c_str());
	mOutputFile.width(15);
	mOutputFile.precision(8);

	mIsRunning = true;
	GridSearch(0);
	mIsRunnning = false;

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
