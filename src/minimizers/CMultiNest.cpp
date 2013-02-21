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

double CMultiNest::ComputeLogZ(valarray<double> & residuals, const valarray<double> & uncertainties)
{
	// We compute the log likelihood from the following formulation:
	//   log Z	= log [Product_i( 1/sqrt(1 pi sigma_i) exp(chi^2_i / 2)]
	//			= -N/2 log(2 pi) - 2 sum_i( log(sigma_i) ) + 1/2 sum_i(chi^2_i)

	// Form the chi^2, store it in the residual buffer
	residuals = residuals*residuals / uncertainties;
	residuals /= 2;

	return -residuals.size() / 2 * log(2*PI) - 2*log(uncertainties).sum() + 0.5 * residuals.sum();
}

CMinimizerPtr CMultiNest::Create()
{
	return shared_ptr<CMinimizer>(new CMultiNest());
}

/// Dumper (do nothing)
void CMultiNest::dumper(int &nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double ** paramConstr, double &maxLogLike, double & logZ, double & logZerr, void * misc)
{
///*
////	 paramConstr(4*nPar):
////   paramConstr(1) to paramConstr(nPar)	     	= mean values of the parameters
////   paramConstr(nPar+1) to paramConstr(2*nPar)    	= standard deviation of the parameters
////   paramConstr(nPar*2+1) to paramConstr(3*nPar)  	= best-fit (maxlike) parameters
////   paramConstr(nPar*4+1) to paramConstr(4*nPar)  	= MAP (maximum-a-posteriori) parameters
//*/
////	printf("npar %i\n", *nPar);
////	printf("Pointer %p\n", paramConstr);
////
////	printf("maxLogLike %f, logZ %f, logZerr %f\n", 	*maxLogLike, *logZ, *logZerr);
////
////	for(int i = 0; i < nPar; i++)
////		printf("%i: %e \n", i, paramConstr[0][i]); //*(paramConstr[2* (nPar) + i]), *(paramConstr[3* (nPar) + i]));
//
//	// If we haven't been requested to stop, copy the values over to a saveable array:
//	if(maxLogLike < numeric_limits<double>::max())
//	{
//		CMultiNest * minimizer = reinterpret_cast<CMultiNest*>(misc);
//		for(int i = 0; i < nPar; i++)
//			minimizer->mParams[i] = paramConstr[0][i];
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
	minimizer->mWorkerThread->GetResiduals(&minimizer->mResiduals[0], minimizer->mResiduals.size());
	lnew = ComputeLogZ(minimizer->mResiduals, minimizer->mUncertainties);

	// TODO: Add in priors
//	lnew += ComputePriors();
}


/// Runs MultiNest.
int CMultiNest::run()
{
	// Init MultiNest:
	string tmp_output = "/tmp"; //mWorkerThread->GetTempOutputDir();

	void * misc = reinterpret_cast<void*>(this);

	// set the MultiNest sampling parameters
	int mmodal = 1;					// do mode separation?
	int ceff = 0;					// run in constant efficiency mode?
	int nlive = 200;				// number of live points
	double efr = 0.8;				// set the required efficiency
	double tol = 1000;				// tol, defines the stopping criteria
	int ndims = mNParams;			// dimensionality (no. of free parameters)
	int nPar = mNParams;				// total no. of parameters including free & derived parameters
	int nClsPar = 1;			// no. of parameters to do mode separation on
	int updInt = 10;				// after how many iterations feedback is required & the output files should be updated
									// note: posterior files are updated & dumper routine is called after every updInt*10 iterations
	double Ztol = -1E90;			// all the modes with logZ < Ztol are ignored
	int maxModes = 10;				// expected max no. of modes (used only for memory allocation)
	int pWrap[ndims];				// which parameters to have periodic boundary conditions?
	for(int i = 0; i < ndims; i++)
	    pWrap[i] = 0;

	const std::string path = mSaveFileBasename + "_multinest";		// root for output files
	int seed = -1;					// random no. generator seed, if < 0 then take the seed from system clock
	int fb = 1;					    // need feedback on standard output?
	int resume = 0;					// resume from a previous job?
	int outfile = 1;				// write output files?
	int initMPI = 0;				// initialize MPI routines?, relevant only if compiling with MPI
							        // set it to F if you want your main program to handle MPI initialization

	double logZero = -numeric_limits<double>::max();		// points with loglike < logZero will be ignored by MultiNest
//	int context = 0;				// not required by MultiNest, any additional information user wants to pass
	int maxIterations = 1E9;

	mIsRunning = true;

    // Run the nested sampling algorithm
    nested::run(mmodal, ceff, nlive, tol,
        efr, ndims, nPar, nClsPar,
        maxModes, updInt, Ztol, path,
        seed, pWrap, fb, resume,
        outfile, initMPI, logZero, maxIterations,
        CMultiNest::log_likelihood,
        CMultiNest::dumper,
        misc);

    mIsRunning = false;

    // TODO: For some reason the parameters are getting mangled when they come from MultiNest
    // resulting in a mangled image for data exporting.
    ExportResults(mParams, mNParams, true);

    return 0;
}
