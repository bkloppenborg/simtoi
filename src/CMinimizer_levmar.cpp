/*
 * CMinimizer_levmar.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: bkloppenborg
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

#include "CMinimizer_levmar.h"
#include <cmath>

#include "levmar.h"
#include "CCL_GLThread.h"


CMinimizer_levmar::CMinimizer_levmar(CCL_GLThread * cl_gl_thread)
: CMinimizer(cl_gl_thread)
{
	mType = CMinimizer::LEVMAR;
	mResiduals = NULL;
}

CMinimizer_levmar::~CMinimizer_levmar()
{
	delete[] mResiduals;
}

void CMinimizer_levmar::ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc)
{
	// Get the "this" pointer
	CMinimizer_levmar * minimizer = reinterpret_cast<CMinimizer_levmar*>(misc);
	int n_data_sets = minimizer->mCLThread->GetNDataSets();
	int n_data_alloc = 0;
	int n_data_offset = 0;

	// See if we have been requested to exit.  If so, give levmar an invalid result
	if(!minimizer->mRun)
	{
		output[0] = 1.0/0;	// Intentional, generate NAN to cause levmar to terminate.
		return;
	}

	// Set the parameters (note, they are already scaled)
	minimizer->mCLThread->SetFreeParameters(params, nParams, false);

	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		n_data_alloc = minimizer->mCLThread->GetNDataAllocated(data_set);
		minimizer->mCLThread->SetTime(minimizer->mCLThread->GetDataAveJD(data_set));
		minimizer->mCLThread->EnqueueOperation(GLT_RenderModels);
		minimizer->mCLThread->GetChi(data_set, minimizer->mResiduals + n_data_offset, n_data_alloc);
		n_data_offset += n_data_alloc;
	}

	// Copy the errors back into the double array:
//	printf("Residuals:\n");
	for(int i = 0; i < nOutput; i++)
	{
		output[i] = double(minimizer->mResiduals[i]);
//		printf("%i %f \n", i, deviates[i]);
	}
}

void CMinimizer_levmar::Init()
{
	CMinimizer::Init();
	int nData = mCLThread->GetNDataAllocated();

	mResiduals = new float[nData];
}

string CMinimizer_levmar::GetExitString(int exit_num)
{
	string tmp;
	switch(exit_num)
	{
	case 1:
		tmp = "1 - stopped by small gradient J^T e";
		break;
	case 2:
		tmp = "2 - stopped by small Dp";
		break;
	case 3:
		tmp = "3 - stopped by itmax";
		break;
	case 4:
		tmp = "4 - singular matrix. Restart from current p with increased mu";
		break;
	case 5:
		tmp = "5 - no further error reduction is possible. Restart with increased mu";
		break;
	case 6:
		tmp = "6 - stopped by small ||e||_2";
		break;
	case 7:
		if(mRun)
			tmp = "7 - stopped by invalid (i.e. NaN or Inf) 'func' values; a user error";
		else
			tmp = "Terminated by User Request.";

		break;
	default:
		tmp =  "Unknown Exit Condition!";
		break;
	}

	return tmp;
}

/// Prints out cmpfit results (from testmpfit.c)
void CMinimizer_levmar::printresult(double * x, int n_pars, int n_data, vector<string> names, double * info, double * covar)
{
	if ((x == 0) || (n_pars == 0))
		return;

	double value = 0;
	double err = 0;
	double scale;
	string name = "";

	printf("Reason for exiting:\n %s\n", GetExitString(int(info[6])).c_str());

	printf("Best-fit parameters:\n");
	for(int i=0; i < n_pars; i++)
	{
		err = sqrt(covar[n_pars * i + i]);
		printf("  P[%d] = %f +/- %f (%s)\n", i, x[i], err, names[i].c_str());
	}

	printf("Covariance Matrix:\n");
	for(int i = 0; i < n_pars; i++)
	{
		for(int j = 0; j < n_pars; j++)
			printf("%1.5e ", covar[n_pars * i + j]);

		printf("\n");
	}

//	if(int(info[6]) == 7 && mRun)
//	{
//		printf("Dumping Residuals Buffer:\n");
//		for(int i = 0; i < n_data; i++)
//			printf(" %i %f\n", i, mResiduals[i]);
//	}

}

int CMinimizer_levmar::run()
{
	// Create a member function pointer
	int iterations = 0;
	int max_iterations = 1E4;
	int nData = mCLThread->GetNDataAllocated();
	mNParams = mCLThread->GetNFreeParameters();
	double x[nData];
	double lb[mNParams];
	double ub[mNParams];
	double info[LM_INFO_SZ];
	double opts[LM_OPTS_SZ];
	double covar[mNParams * mNParams];

	// Setup the options (LM_* from levmar.h):
	// info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
	//  opts[0] = |e||_2 				= LM_INIT_MU = 1E-03
	//  opts[1] = ||J^T e||_inf 		= LM_STOP_THRESH = 1E-15;
	//  opts[2] = ||Dp||_2 				= LM_STOP_THRESH = 1E-15;
	//  opts[3] = mu/max[J^T J]_ii ] 	= LM_STOP_THRESH * LM_STOP_THRESH = 1E-17 * 1E-17;
	//  opts[4]= LM_DIFF_DELTA;
	opts[0]= 1;
	opts[1]= 1E-10;
	opts[2]= 1E-12;
	opts[3]= LM_STOP_THRESH * LM_STOP_THRESH; //1E-9;
	opts[4]= LM_DIFF_DELTA; //1E-9;

	// Copy out the initial values for the parameters:
	mCLThread->GetFreeParameters(mParams, mNParams, true);
	vector<string> names = mCLThread->GetFreeParamNames();
	vector< pair<double, double> > min_max = mCLThread->GetFreeParamMinMaxes();

	// Init parameter500 values
	for(int i = 0; i < mNParams; i++)
	{
		lb[i] = min_max[i].first;
		ub[i] = min_max[i].second;
	}

	printf("Starting levmar...\n");

	mIsRunning = true;

	// Call levmar.  Note, the results are saved in mParams upon completion.
	iterations = dlevmar_bc_dif(&CMinimizer_levmar::ErrorFunc, mParams, x, mNParams, nData, lb, ub, NULL, max_iterations, opts, info, NULL, covar, (void*)this);

	mIsRunning = false;

	printf("Levmar executed %i iterations.\n", iterations);
	printresult(mParams, mNParams, nData, names, info, covar);
	ExportResults(mParams, mNParams);

	return 0;
}
