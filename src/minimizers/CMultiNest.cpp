/*
 * CMultiNest.cpp
 *
 *  Created on: Jan 26, 2012
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

#include "CMultiNest.h"
#include <limits>
#include <cmath>

#include "CWorkerThread.h"
#include "CModelList.h"
#include "textio.hpp"

#ifndef PI
#define PI M_PI
#endif

CMultiNest::CMultiNest()
{
	mMinimizerID = "multinest";
	mMinimizerName = "MultiNest";
}

CMultiNest::~CMultiNest()
{
	// TODO Auto-generated destructor stub
}

/// Computes the prior assuming uniform (uninformative) priors for each parameter
///
/// \param params current parameter values in MultiNest
/// \param n_params the number of parameters in params.
double CMultiNest::ComputePriors(double * params, int n_params)
{
	double prior = 1;

	// Get the parameter min/max values from the model list
	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Compute the prior assuming P(x) = 1 / (x_max - x_min)
	vector< pair<double, double> > min_maxes = model_list->GetFreeParamMinMaxes();
	for(auto min_max : min_maxes)
	{
		prior *= 1.0 / (min_max.second - min_max.first);
	}

	return prior;
}

double CMultiNest::ComputeLogZ(valarray<double> & chis, const valarray<double> & uncertainties)
{
	// We compute the log likelihood from the following formulation:
	//   log Z	= log [Product_i( 1/sqrt(1 pi sigma_i) exp(chi^2_i / 2)]
	//			= -N/2 log(2 pi) - sum_i( log(sigma_i) ) - 1/2 sum_i(chi^2_i)

	// Form the chi squared
	chis *= chis;

	return -0.5 * chis.size() * log(2*PI) - log(uncertainties).sum() - 0.5 * chis.sum();
}

CMinimizerPtr CMultiNest::Create()
{
	return shared_ptr<CMinimizerThread>(new CMultiNest());
}

/// Dumper (do nothing)
void CMultiNest::dumper(int &nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double ** paramConstr, double &maxLogLike, double & logZ, double & logZerr, void * misc)
{
/*
//	 paramConstr(4*nPar):
//   paramConstr(1) to paramConstr(nPar)	     	= mean values of the parameters
//   paramConstr(nPar+1) to paramConstr(2*nPar)    	= standard deviation of the parameters
//   paramConstr(nPar*2+1) to paramConstr(3*nPar)  	= best-fit (maxlike) parameters
//   paramConstr(nPar*4+1) to paramConstr(4*nPar)  	= MAP (maximum-a-posteriori) parameters
*/

	// In our experimentation the dumper doesn't appear to be called with the maximum likelihood parameters
	// at the end of execution. So we'll parse the multinestsummary.txt file instead of attempting
	// to store the best-fit values here.

//	// If we haven't been requested to stop, copy the values over to a save-able array:
//	if(maxLogLike < numeric_limits<double>::max())
//	{
//		CMultiNest * minimizer = reinterpret_cast<CMultiNest*>(misc);
//		CModelListPtr model_list = minimizer->mWorkerThread->GetModelList();
//
//		for(int i = 0; i < nPar; i++)
//			minimizer->mParams[i] = paramConstr[0][i];
//
//		model_list->SetFreeParameters(minimizer->mParams, nPar, true);
//		model_list->GetFreeParameters(minimizer->mParams, nPar, true);
//	}
}

void CMultiNest::log_likelihood(double * params, int & ndim, int & npars, double & lnew, void * misc)
{
	CMultiNest * minimizer = reinterpret_cast<CMultiNest*>(misc);
	double tmp = 0;

	// See if we have been requested to exit.  If so, give MultiNest a very positive result
	if(!minimizer->mRun)
	{
		lnew = numeric_limits<double>::max();
		return;
	}

	// Set the parameters from the unit hypercube and retrieve the scaled values:
	CModelListPtr model_list = minimizer->mWorkerThread->GetModelList();
	model_list->SetFreeParameters(params, npars, true);
	model_list->GetFreeParameters(params, npars, true);

	// Now get the residuals and compute the chi values. Store these in the output double.
	minimizer->mWorkerThread->GetChi(&minimizer->mChis[0], minimizer->mChis.size());
	lnew = ComputeLogZ(minimizer->mChis, minimizer->mUncertainties);
	double temp = lnew;

	// Add in the priors:
	lnew += minimizer->ComputePriors(params, npars);
}

/// Reads in the 'multinestsummary.txt' file and extracts the
void CMultiNest::ResultFromSummaryFile(string multinest_root)
{
	double best_logZ = -numeric_limits<double>::max();
	double logZ = 0;

	vector<string> lines = ReadFile(multinest_root + "summary.txt", "", "Could not read multinestsummary.txt file.");

	for(auto line : lines)
	{
		// Tokenize the line. According to the MultiNest documentation:
		// There is one line per mode with nPar*4+2 values in each line in this file. Each line has the following values
		// in its column mean parameter values, standard deviations of the parameters, bestfit (maxlike) parameter values,
		// MAP (maximum-a-posteriori) parameter values, local log evidence, maximum loglike value
		vector<string> values = Tokenize(line);
		StripWhitespace(values);

		// The logZ value is the last element in each row in the file.
		string temp = values.back();
		logZ = atof(values.back().c_str());

		// if this logZ is better than our best-estimate, copy over the best-fit parameters
		if(logZ > best_logZ)
		{
			best_logZ = logZ;

			for(int i = 0; i < mNParams; i++)
			{
				// Grab the mean parameter values
				mParams[i] = atof(values[i].c_str());
			}
		}

		// As of MultiNest 3.0 the first line contains the global solution, so we only need the first line.
		// TODO: In a future version we should rewrite this function to only use the first line.
		break;

	}

}

/// Runs MultiNest.
void CMultiNest::run()
{
	// Get a copy of the uncertainties from the data
	mWorkerThread->GetUncertainties(&mUncertainties[0], mUncertainties.size());

	// Init MultiNest
	void * misc = reinterpret_cast<void*>(this);

	// set the MultiNest sampling parameters
	int IS = 1;						// do Nested Importance Sampling?
	int mmodal = 0;					// do mode separation?
	int ceff = 0;					// run in constant efficiency mode?
	int nlive = 200;				// number of live points
	double efr = 0.8;				// set the required efficiency
	double tol = 1000;				// tol, defines the stopping criteria
	int ndims = mNParams;			// dimensionality (no. of free parameters)
	int nPar = mNParams;			// total no. of parameters including free & derived parameters
	int nClsPar = 1;				// no. of parameters to do mode separation on
	int updInt = 1;					// after how many iterations feedback is required & the output files should be updated
									// note: posterior files are updated & dumper routine is called after every updInt*10 iterations
	double Ztol = -1E90;			// all the modes with logZ < Ztol are ignored
	int maxModes = 10;				// expected max no. of modes (used only for memory allocation)
	int pWrap[ndims];				// which parameters to have periodic boundary conditions?
	for(int i = 0; i < ndims; i++)
	    pWrap[i] = 0;

	const std::string multinest_root = mSaveDirectory + "/multinest";		// root for output files
	int seed = -1;					// random no. generator seed, if < 0 then take the seed from system clock
	int fb = 1;					    // need feedback on standard output?
	int resume = 0;					// resume from a previous job?
	int outfile = 1;				// write output files?
	int initMPI = 0;				// initialize MPI routines?, relevant only if compiling with MPI
							        // set it to F if you want your main program to handle MPI initialization

	double logZero = -numeric_limits<double>::max();		// points with loglike < logZero will be ignored by MultiNest
//	int context = 0;				// not required by MultiNest, any additional information user wants to pass
	int maxIterations = 1E7;

	mIsRunning = true;

	nested::run(IS, mmodal, ceff, nlive, tol,
			efr, ndims, nPar, nClsPar,
			maxModes, updInt, Ztol, multinest_root,
			seed, pWrap, fb, resume,
			outfile, initMPI, logZero, maxIterations,
	        CMultiNest::log_likelihood,
	        CMultiNest::dumper,
	        misc);

    mIsRunning = false;

    // We should be able to rely on the dumper to get the best-fit parameters
    // at the last MultiNest execution, but this doesn't appear to happen. Instead
    // we will parse the multinestsummary.txt file for the best-fit parameters
    // and set them to mParams prior to running ExportResults.
    ResultFromSummaryFile(multinest_root);

    ExportResults();
}
