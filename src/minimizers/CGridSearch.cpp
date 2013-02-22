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

void CGridSearch::ExportResults(double * params, int n_params, bool no_setparams)
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


void CGridSearch::run()
{
	// TODO: This minimizer only works on two-dimensional data.
	if(mNParams > 2)
		return;

	// Init local storage
	double chi2r = 0;

	// Look up the uncertainties, cache them here.
    mWorkerThread->GetUncertainties(&mUncertainties[0], mUncertainties.size());

    // Get ahold of the model list
    CModelListPtr model_list = mWorkerThread->GetModelList();

	// Get the min/max ranges for the parameters:
    model_list->GetFreeParameters(mParams, mNParams, true);
	vector< pair<double, double> > min_max = model_list->GetFreeParamMinMaxes();

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
			// Permit termination in the middle of a run.
			if(!mRun)
				break;

			// Set the parameters (note, they are not scaled to unit magnitude), get the residuals, compute the chi2r
			model_list->SetFreeParameters(mParams, mNParams, false);
			mWorkerThread->GetResiduals(&mResiduals[0], mResiduals.size());
			chi2r = ComputeChi2r(mResiduals, mUncertainties, mNParams);

			// Enable to see output on the console.
			//cout << mParams[0] << " " << mParams[1]<< " " << chi2r << endl;

			mResults.push_back( tuple<double, double, double>(mParams[0], mParams[1], chi2r) );
		}
	}

	// Export the results.  Note, mParams is set to the maximum value, not the minimum.
	ExportResults(mParams, mNParams, true);
}
