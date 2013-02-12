/*
 * CMinimizer_MultiNest.cpp
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

#include "CMinimizer_MultiNest.h"
#include "CCL_GLThread.h"
#include <limits>

CMinimizer_MultiNest::CMinimizer_MultiNest()
{
	mMinimizerName = "MultiNest";
}

CMinimizer_MultiNest::~CMinimizer_MultiNest()
{
	// TODO Auto-generated destructor stub
}

CMinimizerPtr CMinimizer_MultiNest::Create()
{
	return shared_ptr<CMinimizer>(new CMinimizer_MultiNest());
}

/// Dumper (do nothing)
void CMinimizer_MultiNest::dumper(int &nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double ** paramConstr, double &maxLogLike, double & logZ, double & logZerr, void * misc)
{
/*
//	 paramConstr(4*nPar):
//   paramConstr(1) to paramConstr(nPar)	     	= mean values of the parameters
//   paramConstr(nPar+1) to paramConstr(2*nPar)    	= standard deviation of the parameters
//   paramConstr(nPar*2+1) to paramConstr(3*nPar)  	= best-fit (maxlike) parameters
//   paramConstr(nPar*4+1) to paramConstr(4*nPar)  	= MAP (maximum-a-posteriori) parameters
*/
//	printf("npar %i\n", *nPar);
//	printf("Pointer %p\n", paramConstr);
//
//	printf("maxLogLike %f, logZ %f, logZerr %f\n", 	*maxLogLike, *logZ, *logZerr);
//
//	for(int i = 0; i < nPar; i++)
//		printf("%i: %e \n", i, paramConstr[0][i]); //*(paramConstr[2* (nPar) + i]), *(paramConstr[3* (nPar) + i]));

	// If we haven't been requested to stop, copy the values over to a saveable array:
	if(maxLogLike < numeric_limits<double>::max())
	{
		CMinimizer_MultiNest * minimizer = reinterpret_cast<CMinimizer_MultiNest*>(misc);
		for(int i = 0; i < nPar; i++)
			minimizer->mParams[i] = paramConstr[0][i];
	}
}

void CMinimizer_MultiNest::log_likelihood(double * params, int & ndim, int & npars, double & lnew, void * misc)
{
	CMinimizer_MultiNest * minimizer = reinterpret_cast<CMinimizer_MultiNest*>(misc);
	int n_data_sets = minimizer->mCLThread->GetNDataSets();
	double tmp = 0;

	// See if we have been requested to exit.  If so, give MultiNest a very positive result
	if(!minimizer->mRun)
	{
		lnew = numeric_limits<double>::max();
		return;
	}

	// Convert the double parameter values back to floats
	int nData = minimizer->mCLThread->GetNData();

	minimizer->mCLThread->SetFreeParameters(params, npars, true);
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		minimizer->mCLThread->SetTime(minimizer->mCLThread->GetDataAveJD(data_set));
		minimizer->mCLThread->EnqueueOperation(GLT_RenderModels);
		tmp += minimizer->mCLThread->GetLogLike(data_set);
	}

	// Add in the priors.
	tmp += minimizer->mCLThread->GetFreeParameterPriorProduct();

	// Get the scaled parameter values
	minimizer->mCLThread->GetFreeParameters(params, npars, true);

	lnew = tmp;
}


/// Runs MultiNest.
int CMinimizer_MultiNest::run()
{
	// Init MultiNest:
	mNParams = mCLThread->GetNFreeParameters();
	//string tmp_output = mCLThread->GetTempOutputDir();

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

	double logZero = -DBL_MAX;		// points with loglike < logZero will be ignored by MultiNest
//	int context = 0;				// not required by MultiNest, any additional information user wants to pass
	int maxIterations = 1E9;

	mIsRunning = true;

    // Run the nested sampling algorithm
    nested::run(mmodal, ceff, nlive, tol,
        efr, ndims, nPar, nClsPar,
        maxModes, updInt, Ztol, path,
        seed, pWrap, fb, resume,
        outfile, initMPI, logZero, maxIterations,
        CMinimizer_MultiNest::log_likelihood,
        CMinimizer_MultiNest::dumper,
        misc);

    mIsRunning = false;

    // TODO: For some reason the parameters are getting mangled when they come from MultiNest
    // resulting in a mangled image for data exporting.
    ExportResults(mParams, mNParams, true);

    return 0;
}
