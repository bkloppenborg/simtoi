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
#include <stdexcept>

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

	mBootstrapFailures = 0;
	mMaxBootstrapFailures = 20;
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
	filename << mSaveFileBasename << "_bootstrap.txt";
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

	// Call the base class function to export the parameter names.
	CMinimizer::ExportResults(params, n_params, no_setparams);

}

void CMinimizer_Bootstrap::Init()
{
	CMinimizer_levmar::Init();

	// Get a copy of the original data, load it into memory.
	int nData = mCLThread->GetNDataSets();
	for(int data_set = 0; data_set < nData; data_set++)
		mData.push_back(mCLThread->GetData(data_set));
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

	// Recalibrate the data.
	// TODO: Note this function assumes the SAME calibrator is used on ALL data sets. This probably isn't true.
	// This issue is listed in https://github.com/bkloppenborg/simtoi/issues/53.
	for(int i = 0; i < mData.size(); i++)
	{
		auto data = mData[i];

		// Recalibrate, bootstrap, then push to the OpenCL device:
		OIDataList t_data = Recalibrate(data, old_cal, new_cal);
		t_data = Bootstrap_Spectral(t_data);

		// Replace the 0th entry with temp.
		// Due to a bug in ccoifits the total data size may not be preserved, so we need to
		// catch and repeat.
		try
		{
			mCLThread->ReplaceData(i, t_data);
		}
		catch(length_error& l)
		{
			// Generate an error message on stderr.
			cerr << " Warning: " << l.what() << " " << "Generating a new bootstrapped data set." << endl;
			mBootstrapFailures += 1;
			i--;

			// If we haven't exceeded the maximum number of bootstrap failures, repeat
			// this iteration.
			if(mBootstrapFailures < mMaxBootstrapFailures)
				continue;
			else
				throw runtime_error("Too many bootstrap data generation failures.");
		}
		mBootstrapFailures = 0;
	}
}

int CMinimizer_Bootstrap::run()
{
	// init local storage
	vector<double> tmp_vec;
	long double tmp_chi2 = 0;
	double chi2r_ave = 0;
	double tmp = 0;
	int exit_value = 0;
	int iterations = 10000;
	// The maximum chi2r that will be accepted. Iterations exceeding this value will be repeated.
	float chi2_threshold = 10;
	int chi2r_exceeded = 0;
	int nData = 0;

	// Setup the random number generator:
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	uniform_real_distribution<double> distribution (0.0,1.0);
	vector< pair<double, double> > min_max = mCLThread->GetFreeParamMinMaxes();

	for(int iteration = 0; iteration < iterations; iteration++)
	{
		if(!mRun)
			break;

		// Randomize the starting position of the minimizer
		for(int i = 0; i < mNParams; i++)
			mParams[i] = distribution(generator);

		// Set the starting position.  Note these values are [0...1] and need to be scaled.
		mCLThread->SetFreeParameters(mParams, mNParams, true);

		// Print a status message:
		cout << endl << "Starting iteration " << iteration + 1 << endl;

		// run the minimizer
		exit_value = CMinimizer_levmar::run(&CMinimizer_Bootstrap::ErrorFunc);

		// Compute the average reduced chi2 per data set:
		chi2r_ave = 0;
		tmp_chi2 = 0;
		mCLThread->SetFreeParameters(mParams, mNParams, false);
		for(int data_set = 0; data_set < mData.size(); data_set++)
		{
			nData = mCLThread->GetNDataAllocated(data_set);
			mCLThread->SetTime(mCLThread->GetDataAveJD(data_set));
			mCLThread->EnqueueOperation(GLT_RenderModels);
			mCLThread->GetChi(data_set, mResiduals, nData);

			tmp_chi2 = 0;
			tmp = 0;
			for(int i = 0; i < nData; i++)
				tmp_chi2 += mResiduals[i] * mResiduals[i];

			// Compute the reduced chi2 for this data set and add it to the average
			chi2r_ave += tmp_chi2 / (nData - mNParams - 1);
		}
		// Divide by the number of data sets to compute the average.
		chi2r_ave /= mData.size();

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

		// Save the results.  Start by copying the current entry to the temporary vector:
		tmp_vec.clear();
		for(int i = 0; i < mNParams; i++)
			tmp_vec.push_back(mParams[i]);

		// append the chi2
		tmp_vec.push_back(tmp_chi2);

		// push this vector onto the back of mResults
		mResults.push_back(tmp_vec);

		// Get the next bootstrapped data set and repeat.
		Next();
	}

	// Save the results of the bootstrapper.
    ExportResults(mParams, mNParams, true);

	return 0;
}
