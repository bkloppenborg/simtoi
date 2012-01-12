/*
 * CMinimizer_MultiNest.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: bkloppenborg
 */

#include "CMinimizer_MultiNest.h"
#include "CSIMTOI.h"
#include <float.h>
#include <cstring>
#include "multinest.h"

CMinimizer_MultiNest::CMinimizer_MultiNest(CSIMTOI * simtoi)
	:CMinimizer(simtoi)
{
	mOutput = "/home/bkloppenborg/workspace/simtoi/bin/chains/multinest";
}

CMinimizer_MultiNest::~CMinimizer_MultiNest()
{
	// TODO Auto-generated destructor stub
}

/// The function to generate the loglikelihood.
void CMinimizer_MultiNest::log_likelihood(double * Cube, int * ndim, int * npars, double * lnew)
{
	int n = *ndim;
	// First repackage the parameters as a float array:
	float * tmp0 = new float[n];
	for(int i = 0; i < n; i++)
		tmp0[i] = (float) Cube[i];

	CSIMTOI * simtoi = CSIMTOI::GetInstance();

	// Now set the parameters and instruct the models to render:
	simtoi->SetParameters(tmp0, n);
	simtoi->Render();

	// TODO: If we are working with multi-wavelength data and/or multiple data sets, we will need to change this:
    double tmp1 = (double) simtoi->GetLogLike(0, 0);
    double tmp2 = 0; //(double) mSIMTOI->GetPrior(0, 0);

    // Assign the value and we're done.
	*lnew = tmp1 + tmp2;

	// Free memory:
	delete tmp0;
}

// Dumper, do nothing.
void CMinimizer_MultiNest::dumper(int *nSamples, int *nlive, int *nPar, double **physLive, double **posterior, double *paramConstr, double *maxLogLike, double *logZ, double *logZerr)
{

}

/// Runs the Multinest minimization thread.
int CMinimizer_MultiNest::ThreadFunc()
{
	// Pad the end of the output string to 100 characters (multinest requirement)
	mOutput.reserve(100);

	// set the MultiNest sampling parameters
	int mmodal = 1;					// do mode separation?
	int ceff = 0;					// run in constant efficiency mode?
	int nlive = 1000;				// number of live points
	double efr = 1.0;				// set the required efficiency
	double tol = 0.5;				// tol, defines the stopping criteria
	int ndims = mSIMTOI->GetNFreeParameters();		// dimensionality (no. of free parameters)
	int nPar = mSIMTOI->GetNFreeParameters();		// total no. of parameters including free & derived parameters
	int nClsPar = mSIMTOI->GetNFreeParameters();	// no. of parameters to do mode separation on
	int updInt = 100;				// after how many iterations feedback is required & the output files should be updated
									// note: posterior files are updated & dumper routine is called after every updInt*10 iterations
	double Ztol = -1E90;			// all the modes with logZ < Ztol are ignored
	int maxModes = 100;				// expected max no. of modes (used only for memory allocation)
	int * pWrap = new int[ndims];				// which parameters to have periodic boundary conditions?
	for(int i = 0; i < ndims; i++)
	    pWrap[i] = 0;

	char root[100] = "/home/bkloppenborg/workspace/simtoi/bin/chains/multinest";
	//char * root = (char*) mOutput.c_str();	// root for output files
	for (int i = strlen(root); i < 100; i++) root[i] = ' ';

	int seed = -1;					// random no. generator seed, if < 0 then take the seed from system clock
	int fb = 1;					    // need feedback on standard output?
	int resume = 0;					// resume from a previous job?
	int outfile = 1;				// write output files?
	int initMPI = 1;				// initialize MPI routines?, relevant only if compiling with MPI
							        // set it to F if you want your main program to handle MPI initialization

	double logZero = -DBL_MAX;		// points with loglike < logZero will be ignored by MultiNest
	int context = 0;				// not required by MultiNest, any additional information user wants to pass


	// Run the nested sampling algorithm
	NESTRUN(&mmodal, &ceff, &nlive, &tol,
		&efr, &ndims, &nPar, &nClsPar,
		&maxModes, &updInt, &Ztol, root,
		&seed, pWrap, &fb, &resume,
		&outfile, &initMPI, &logZero,
		&this->log_likelihood,
		&this->dumper,
		&context);
}
