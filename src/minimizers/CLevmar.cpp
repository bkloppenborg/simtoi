/*
 * Clevmar.cpp
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

#include "CLevmar.h"
#include <cmath>

#include "levmar.h"
#include "CWorkerThread.h"


CLevmar::CLevmar()
{
	mMinimizerID = "levmar";
	mMinimizerName = "Levmar";
	mResiduals = NULL;
}

CLevmar::~CLevmar()
{
	if(mResiduals) delete[] mResiduals;
}

CMinimizerPtr CLevmar::Create()
{
	return shared_ptr<CMinimizer>(new CLevmar());
}

void CLevmar::ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc)
{
//	// Get the "this" pointer
//	CLevmar * minimizer = reinterpret_cast<CLevmar*>(misc);
//	int n_data_sets = minimizer->mWorkerThread->GetNDataSets();
//	int n_data_alloc = 0;
//	int n_data_offset = 0;
//
//	// See if we have been requested to exit.  If so, give levmar an invalid result
//	if(!minimizer->mRun)
//	{
//		output[0] = 1.0/0;	// Intentional, generate NAN to cause levmar to terminate.
//		return;
//	}
//
//	// Set the parameters (note, they are already scaled)
//	minimizer->mWorkerThread->SetFreeParameters(params, nParams, false);
//
//	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
//	for(int data_set = 0; data_set < n_data_sets; data_set++)
//	{
//		n_data_alloc = minimizer->mWorkerThread->GetNDataAllocated(data_set);
//		minimizer->mWorkerThread->SetTime(minimizer->mWorkerThread->GetDataAveJD(data_set));
//		minimizer->mWorkerThread->EnqueueOperation(GLT_RenderModels);
//		minimizer->mWorkerThread->GetChi(data_set, minimizer->mResiduals + n_data_offset, n_data_alloc);
//		n_data_offset += n_data_alloc;
//	}
//
//	// Copy the errors back into the double array:
////	printf("Residuals:\n");
//	for(int i = 0; i < nOutput; i++)
//	{
//		output[i] = double(minimizer->mResiduals[i]);
////		cout << i << " " << output[i] << endl;
//	}
}

void CLevmar::Init(shared_ptr<CWorkerThread> worker_thread)
{
	CMinimizer::Init(worker_thread);
//	int nData = mWorkerThread->GetNDataAllocated();
//
//	mResiduals = new float[nData];
//	for(int i = 0; i < nData; i++)
//	{
//		mResiduals[i] = 0;
//	}
}

string CLevmar::GetExitString(int exit_num)
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
void CLevmar::printresult(double * x, int n_pars, int n_data, vector<string> names, valarray<double> & info, valarray<double> & covar)
{
//	if ((x == 0) || (n_pars == 0))
//		return;
//
//	// Print out some statistics:
//	printf("Chi2r for each data set:\n");
//	int nData = 0;
//	double chi2r_total = 0;
//	double chi2r = 0;
//	int nDataSets = mWorkerThread->GetNDataSets();
//	mWorkerThread->SetFreeParameters(mParams, mNParams, false);
//	for(int data_set = 0; data_set < nDataSets; data_set++)
//	{
//		nData = mWorkerThread->GetNDataAllocated(data_set);
//		mWorkerThread->SetTime(mWorkerThread->GetDataAveJD(data_set));
//		mWorkerThread->EnqueueOperation(GLT_RenderModels);
//		chi2r = mWorkerThread->GetChi2(data_set) / (nData - mNParams - 1);
//		chi2r_total += chi2r;
//		printf("  Data Set %i chi2r: %f\n", data_set, chi2r);
//	}
//	printf("All data, average chi2r: %f\n", chi2r_total/nDataSets);
//
//	double value = 0;
//	double err = 0;
//	double scale;
//	string name = "";
//
//	printf("Reason for exiting:\n %s\n", GetExitString(int(info[6])).c_str());
//
//	printf("Best-fit parameters:\n");
//	for(int i=0; i < n_pars; i++)
//	{
//		err = sqrt(covar[n_pars * i + i]);
//		printf("  P[%d] = %f +/- %f (%s)\n", i, x[i], err, names[i].c_str());
//	}
//
//	printf("Covariance Matrix:\n");
//	for(int i = 0; i < n_pars; i++)
//	{
//		for(int j = 0; j < n_pars; j++)
//			printf("%1.5e ", covar[n_pars * i + j]);
//
//		printf("\n");
//	}
//
////	if(int(info[6]) == 7 && mRun)
////	{
////		printf("Dumping Residuals Buffer:\n");
////		for(int i = 0; i < n_data; i++)
////			printf(" %i %f\n", i, mResiduals[i]);
////	}

}

int CLevmar::run()
{
	// Run the minimizer using this instance of CMinimizer_levmar
	run(&CLevmar::ErrorFunc);
}

int CLevmar::run(void (*error_func)(double *p, double *hx, int m, int n, void *adata))
{
//	// Ensure memory is initialized.
//	if(mResiduals == NULL)
//		return -1;
//
//	// Create a member function pointer
//	int iterations = 0;
//	int max_iterations = 50;
//	int nData = mWorkerThread->GetNDataAllocated();
//	valarray<double> x(nData);
//	valarray<double> lb(mNParams);
//	valarray<double> ub(mNParams);
//	valarray<double> info(LM_INFO_SZ);
//	valarray<double> opts(LM_INFO_SZ);
//	valarray<double> covar(mNParams * mNParams);
//
//	// Setup the options (LM_* from levmar.h):
//	// info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
//	// \tau: scale factor for initial \mu
//	//  opts[0] = |e||_2 				= LM_INIT_MU = 1E-03
//	// \epsilon1: stopping thresholds for ||J^T e||_inf
//	//  opts[1] = ||J^T e||_inf 		= LM_STOP_THRESH = 1E-15;
//	// \epsilon2: stopping thresholds for ||Dp||_2
//	//  opts[2] = ||Dp||_2 				= LM_STOP_THRESH = 1E-15;
//	// \epsilon3: stopping thresholds for ||e||_2
//	//  opts[3] = mu/max[J^T J]_ii ] 	= LM_STOP_THRESH * LM_STOP_THRESH = 1E-17 * 1E-17;
//	// \delta, step used in difference approximation to the Jacobian.  \delta < 0 => central difference instead of forward difference used
//	//  opts[4]= LM_DIFF_DELTA;
//	opts[0]= 1;
//	opts[1]= 1E-4;
//	opts[2]= 1E-4;
//	opts[3]= 1E-12;
//	opts[4]= LM_DIFF_DELTA;
//
//	// Copy out the initial values for the parameters:
//	mWorkerThread->GetFreeParameters(mParams, mNParams, true);
//	vector<string> names = mWorkerThread->GetFreeParamNames();
//	vector< pair<double, double> > min_max = mWorkerThread->GetFreeParamMinMaxes();
//
//	// Init parameter500 values
//	for(int i = 0; i < mNParams; i++)
//	{
//		lb[i] = min_max[i].first;
//		ub[i] = min_max[i].second;
//	}
//
//	printf("Starting levmar...\n");
//
//	mIsRunning = true;
//
//	// Call levmar.  Note, the results are saved in mParams upon completion.
//	iterations = dlevmar_bc_dif(error_func, mParams, &x[0], mNParams, nData, &lb[0], &ub[0], NULL, max_iterations, &opts[0], &info[0], NULL, &covar[0], (void*)this);
//
//	mIsRunning = false;
//
//	printf("Levmar executed %i iterations.\n", iterations);
//	printresult(mParams, mNParams, nData, names, info, covar);
//	ExportResults(mParams, mNParams);
//
//	return iterations;
}
