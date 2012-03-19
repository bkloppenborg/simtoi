/*
 * CMinimizer_MultiNest.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_MultiNest.h"
#include "CCL_GLThread.h"
#include <float.h>

CMinimizer_MultiNest::CMinimizer_MultiNest(CCL_GLThread * cl_gl_thread)
	: CMinimizer(cl_gl_thread)
{
	mType = CMinimizer::MULTINEST;
}

CMinimizer_MultiNest::~CMinimizer_MultiNest()
{
	// TODO Auto-generated destructor stub
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
//	for(int i = 0; i < *nPar + 1; i++)
//		printf("%s: %e %e %e\n", param_names[i].c_str(), paramConstr[i], paramConstr[2* (*nPar) + i], paramConstr[3* (*nPar) + i]);
}

void CMinimizer_MultiNest::log_likelihood(double * params, int & ndim, int & npars, double & lnew, void * misc)
{
	CMinimizer_MultiNest * minimizer = reinterpret_cast<CMinimizer_MultiNest*>(misc);
	int n_data_sets = minimizer->mCLThread->GetNDataSets();
	double tmp = 0;

	// See if we have been requested to exit.  If so, give MultiNest a very positive result
	if(!minimizer->mRun)
	{
		lnew = 1E99;
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
	minimizer->mCLThread->GetFreeParameters(params, npars, true);;

	lnew = tmp;
}


/// Runs MultiNest.
int CMinimizer_MultiNest::run()
{
	// Init MultiNest:
	int nParams = mCLThread->GetNFreeParameters();
	//string tmp_output = mCLThread->GetTempOutputDir();

	void * misc = reinterpret_cast<void*>(this);

	// set the MultiNest sampling parameters
	int mmodal = 1;					// do mode separation?
	int ceff = 0;					// run in constant efficiency mode?
	int nlive = 200;				// number of live points
	double efr = 1.0;				// set the required efficiency
	double tol = 0.9;				// tol, defines the stopping criteria
	int ndims = nParams;			// dimensionality (no. of free parameters)
	int nPar = nParams;				// total no. of parameters including free & derived parameters
	int nClsPar = nParams;			// no. of parameters to do mode separation on
	int updInt = 10;				// after how many iterations feedback is required & the output files should be updated
									// note: posterior files are updated & dumper routine is called after every updInt*10 iterations
	double Ztol = -1E90;			// all the modes with logZ < Ztol are ignored
	int maxModes = 100;				// expected max no. of modes (used only for memory allocation)
	int pWrap[ndims];				// which parameters to have periodic boundary conditions?
	for(int i = 0; i < ndims; i++)
	    pWrap[i] = 0;

	const std::string path = "/tmp/mn";		// root for output files
	int seed = -1;					// random no. generator seed, if < 0 then take the seed from system clock
	int fb = 1;					    // need feedback on standard output?
	int resume = 0;					// resume from a previous job?
	int outfile = 1;				// write output files?
	int initMPI = 0;				// initialize MPI routines?, relevant only if compiling with MPI
							        // set it to F if you want your main program to handle MPI initialization

	double logZero = -DBL_MAX;		// points with loglike < logZero will be ignored by MultiNest
	int context = 0;				// not required by MultiNest, any additional information user wants to pass

	mIsRunning = true;

    // Run the nested sampling algorithm
    nested::run(mmodal, ceff, nlive, tol,
        efr, ndims, nPar, nClsPar,
        maxModes, updInt, Ztol, path,
        seed, pWrap, fb, resume,
        outfile, initMPI, logZero,
        CMinimizer_MultiNest::log_likelihood,
        CMinimizer_MultiNest::dumper,
        misc);

    mIsRunning = false;

    return 0;
}
