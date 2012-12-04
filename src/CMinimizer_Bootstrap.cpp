/*
 * CMinimizer_Bootstrap.cpp
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

#include "CMinimizer_Bootstrap.h"

#include <random>
#include <chrono>
#include "levmar.h"
#include "CCL_GLThread.h"

#include "oi_tools.hpp"
#include "CUniformDisk.h"

using namespace std;


CMinimizer_Bootstrap::CMinimizer_Bootstrap(CCL_GLThread * cl_gl_thread)
: CMinimizer_levmar(cl_gl_thread)
{
	mType = CMinimizer::BOOTSTRAP;
	mResiduals = NULL;
}

CMinimizer_Bootstrap::~CMinimizer_Bootstrap()
{

}

///
void CMinimizer_Bootstrap::ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc)
{
	// First run the standard levmar minimizer:
	CMinimizer_levmar::ErrorFunc(params, output, nParams, nOutput, misc);
}

void CMinimizer_Bootstrap::ExportResults(double * params, int n_params, bool no_setparams)
{
	stringstream filename;
	ofstream outfile;
	double tmp[3];

	// Open the statistics file for writing:
	filename.str("");
	filename << mResultsBaseFilename << "_bootstrap.txt";
	outfile.open(filename.str().c_str());
	outfile.width(15);
	outfile.precision(8);
	outfile << "# Param1 Param2 ... ParamN Chi2/(nData - nParams - 1)" << endl;

	// write the data to the file
	WriteTable(mResults, outfile);
	outfile.close();

//	// now write out the masks
//	filename.str("");
//	filename << mResultsBaseFilename << "_bootstrap_mask.txt";
//	outfile.open(filename.str().c_str());
//	outfile.width(3);
//	outfile.precision(0);
//	outfile << "# Bootstrap masks on the data.  Each row is one mask designated by:" << endl;
//	outfile << "# Data0 ... DataN" << endl;
//	WriteTable(mMasks, outfile);
//	outfile.close();

}

void CMinimizer_Bootstrap::Init()
{
	CMinimizer_levmar::Init();

	// Get a copy of the zeroth data loaded in memory.
	mData = mCLThread->GetData(0);
}

void CMinimizer_Bootstrap::Next()
{

// TODO: Calibrator information is hard-coded for eps Aur. This should be read in from elsewhere.
	pair<double, double> cal_diam(0.419 * MAS_TO_RAD, 0.063 * MAS_TO_RAD);

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(cal_diam.first, cal_diam.second);

	// Setup the old calibrator
	OICalibratorPtr old_cal = OICalibratorPtr( new ccoifits::CUniformDisk(cal_diam.first) );
	OICalibratorPtr new_cal = OICalibratorPtr( new ccoifits::CUniformDisk(distribution(generator)) );

	// Recalibrate, bootstrap, then push to the OpenCL device:
	OIDataList temp = Recalibrate(mData, old_cal, new_cal);
	temp = Bootstrap_Spectral(temp);

	// Replace the 0th entry with temp.
	mCLThread->ReplaceData(0, temp);
}

int CMinimizer_Bootstrap::run()
{
	// init local storage
	vector<double> tmp_vec;
	long double tmp_chi2 = 0;
	double tmp = 0;
	int exit_value = 0;
	int nBootstrap = 1000;
	int nData = mCLThread->GetNDataAllocated();

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	uniform_real_distribution<double> distribution (0.0,1.0);
	vector< pair<double, double> > min_max = mCLThread->GetFreeParamMinMaxes();

	for(int iteration = 0; iteration < nBootstrap; iteration++)
	{
		if(!mRun)
			break;

		// Randomize the starting position of the minimizer
		for(int i = 0; i < mNParams; i++)
			mParams[i] = distribution(generator);

		// Set the starting position.  Note these values are [0...1] and need to be scaled.
		mCLThread->SetFreeParameters(mParams, mNParams, true);

		// Print a status message:
		cout << "Starting iteration " << iteration + 1 << endl;

		// run the minimizer
		exit_value = CMinimizer_levmar::run(&CMinimizer_Bootstrap::ErrorFunc);

		// Get the current chi2:
		mCLThread->SetFreeParameters(mParams, mNParams, false);
		mCLThread->SetTime(mCLThread->GetDataAveJD(0));
		mCLThread->EnqueueOperation(GLT_RenderModels);
		mCLThread->GetChi(0, mResiduals, nData);

		tmp_chi2 = 0;
		tmp = 0;
		for(int i = 0; i < nData; i++)
			tmp_chi2 += mResiduals[i]*mResiduals[i];

		// convert to a reduced chi2
		tmp_chi2 /= (nData - mNParams - 1);

		// Save the results.  Start by copying the current entry to the temporary vector:
		tmp_vec.clear();
		for(int i = 0; i < mNParams; i++)
			tmp_vec.push_back(mParams[i]);

		// append the chi2
		tmp_vec.push_back(tmp_chi2);

		// push this vector onto the back of mResults
		mResults.push_back(tmp_vec);

		// Get the next bootstrapped data set:
		Next();
	}

	// Save the results of the bootstrapper.
    ExportResults(mParams, mNParams, true);

	return 0;
}
