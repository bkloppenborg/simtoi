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

using namespace std;


CMinimizer_Bootstrap::CMinimizer_Bootstrap(CCL_GLThread * cl_gl_thread)
: CMinimizer_levmar(cl_gl_thread)
{
	mType = CMinimizer::BOOTSTRAP;
	mResiduals = NULL;
	mMask = NULL;
}

CMinimizer_Bootstrap::~CMinimizer_Bootstrap()
{
	delete[] mMask;
}

///
void CMinimizer_Bootstrap::ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc)
{
	// First run the standard levmar minimizer:
	CMinimizer_levmar::ErrorFunc(params, output, nParams, nOutput, misc);

	// Now apply a mask to the output:
	CMinimizer_Bootstrap * minimizer = reinterpret_cast<CMinimizer_Bootstrap*>(misc);
	for(int i = 0; i < nOutput; i++)
		output[i] *= minimizer->mMask[i];
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
	outfile << "# Param1 Param2 ... ParamN Chi2/N(params)" << endl;

	// write the data to the file
	WriteTable(mResults, outfile);
	outfile.close();

	// now write out the masks
	filename.str("");
	filename << mResultsBaseFilename << "_bootstrap_mask.txt";
	outfile.open(filename.str().c_str());
	outfile.width(3);
	outfile.precision(0);
	outfile << "# Mask1 Mask ... MaskN" << endl;
	WriteTable(mMasks, outfile);
	outfile.close();

}

void CMinimizer_Bootstrap::Init()
{
	CMinimizer_levmar::Init();
	int nData = mCLThread->GetNDataAllocated();
	mMask = new unsigned int[nData];

	for(int i = 0; i < nData; i++)
		mMask[i] = 1;
}

void CMinimizer_Bootstrap::NextMask()
{
	// Init to all zeros:
	int n_data = mCLThread->GetNDataAllocated();
	int n_v2 = mCLThread->GetNV2(0);
	int n_t3 = mCLThread->GetNT3(0);
	int j = 0;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator (seed);

	uniform_int_distribution<int> v2_distribution(0, n_v2);
	uniform_int_distribution<int> t3_distribution(0, n_t3);


	for(int i = 0; i < n_data; i++)
		mMask[i] = 0;

	// Randomly select the V2 that will be activated:
	for(int i = 0; i < n_v2; i++)
	{
		j = v2_distribution(generator);
		mMask[j] += 1;
	}

	for(int i = 0; i < n_t3; i++)
	{
		j = t3_distribution(generator);
		mMask[n_v2 + 2*j  ] += 1;
		mMask[n_v2 + 2*j+1] += 1;
	}

	vector<unsigned int> tmp;
	// Copy the mask for saving
	for(int i = 0; i < n_data; i++)
		tmp.push_back(mMask[i]);

	mMasks.push_back(tmp);
}

int CMinimizer_Bootstrap::run()
{
//	// Ensure memory is initialized.
//	if(mResiduals == NULL)
//		return -1;

	// init local storage
	vector<double> tmp_vec;
	long double tmp_chi2 = 0;
	double tmp = 0;
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

		// Get the next mask:
		NextMask();

		// Randomize the starting position:
		for(int i = 0; i < mNParams; i++)
			mParams[i] = distribution(generator);

		// Set the starting position.  Note these values are [0...1] and need to be scaled.
		mCLThread->SetFreeParameters(mParams, mNParams, true);

//		// Print a status message:
//		mCLThread->GetFreeParameters(mParams, mNParams, true);
//		printf("Starting iteration %i with values: ", iteration);
//		// Draw a random number between 0 and 1:
//		for(int i = 0; i < mNParams; i++)
//			printf("%f ", mParams[i]);
//
//		printf("\n");

		// run the minimizer
		int exit_value = CMinimizer_levmar::run(&CMinimizer_Bootstrap::ErrorFunc);

		if(exit_value < 0)
			break;

		// Get the current chi2:
		mCLThread->SetFreeParameters(mParams, mNParams, false);
		mCLThread->SetTime(mCLThread->GetDataAveJD(0));
		mCLThread->EnqueueOperation(GLT_RenderModels);

		mCLThread->GetChi(0, mResiduals, nData);

		tmp_chi2 = 0;
		tmp = 0;
		for(int i = 0; i < nData; i++)
		{
			tmp = mResiduals[i] * mMask[i];
			tmp_chi2 += tmp*tmp;
		}
		// convert to reduced chi2
		tmp_chi2 /= mNParams;

		// Save the results.  Start by copying the current entry to the temporary vector:
		tmp_vec.clear();
		for(int i = 0; i < mNParams; i++)
			tmp_vec.push_back(mParams[i]);

		// append the chi2
		tmp_vec.push_back(tmp_chi2);

		// push this vector onto the back of mResults
		mResults.push_back(tmp_vec);
	}

	// Save the results of the bootstrapper.
    ExportResults(mParams, mNParams, true);

	return 0;
}
