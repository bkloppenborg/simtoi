/*
 * CMinimizer_GridSearch.cpp
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

#include "CMinimizer_GridSearch.h"

#include <tuple>
#include <fstream>
#include <sstream>

#include "CCL_GLThread.h"

using namespace std;

CMinimizer_GridSearch::CMinimizer_GridSearch()
{
	mMinimizerName = "Gridsearch";
}

CMinimizer_GridSearch::~CMinimizer_GridSearch() {
	// TODO Auto-generated destructor stub
}

CMinimizerPtr CMinimizer_GridSearch::Create()
{
	return shared_ptr<CMinimizer>(new CMinimizer_GridSearch());
}

void CMinimizer_GridSearch::ExportResults(double * params, int n_params, bool no_setparams)
{
	stringstream filename;
	ofstream outfile;
	double tmp[3];

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveFileBasename << "_gridsearch.txt";
	outfile.open(filename.str().c_str());
	outfile.width(15);
	outfile.precision(8);
	outfile << "# Param1 Param2 Chi2" << endl;

	// write the data to the file
	for(int i = 0; i < mResults.size(); i++)
		outfile << get<0>(mResults[i]) << " " << get<1>(mResults[i]) << " " << get<2>(mResults[i]) << endl;

	outfile.close();
}


int CMinimizer_GridSearch::run()
{
	// TODO: This minimizer only works on two-dimentional data.
	if(mNParams > 2)
		return 0;

	int nData = 0;
	int nDataSets = mCLThread->GetNDataSets();
	int n_data_offset = 0;
	double chi2r_sum = 0;

	// Get the min/max ranges for the parameters:
	mCLThread->GetFreeParameters(mParams, mNParams, true);
	vector< pair<double, double> > min_max = mCLThread->GetFreeParamMinMaxes();

	// Hard coded for the moment, results in 2500 iterations equally spaced in both dimensions.
	int nSteps = 50;

	// Determine the size of each step.
	double steps[mNParams];
	for(int i = 0; i < mNParams; i++)
		steps[i] = (min_max[i].second - min_max[i].first) / nSteps;

	// Run the grid search.
	for(mParams[0] = min_max[0].first; mParams[0] < min_max[0].second; mParams[0] += steps[0])
	{
		for(mParams[1] = min_max[1].first; mParams[1] < min_max[1].second; mParams[1] += steps[1])
		{
			chi2r_sum = 0;
			// Permit termination in the middle of a run.
			if(!mRun)
				break;

			// Set the parameters (note, they are not scaled to unit magnitude).
			mCLThread->SetFreeParameters(mParams, mNParams, false);

			// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
			for(int data_set = 0; data_set < nDataSets; data_set++)
			{
				nData = mCLThread->GetNDataAllocated(data_set);
				mCLThread->SetTime(mCLThread->GetDataAveJD(data_set));
				mCLThread->EnqueueOperation(GLT_RenderModels);
				chi2r_sum += mCLThread->GetChi2(data_set) / (nData - mNParams - 1);
				n_data_offset += nData;
			}

			mResults.push_back( tuple<double, double, double>(mParams[0], mParams[1], chi2r_sum/nDataSets) );

		}
	}

	// Export the results.  Note, mParams is set to the maximum value, not the minimum.
	ExportResults(mParams, mNParams, true);
	return 0;
}



