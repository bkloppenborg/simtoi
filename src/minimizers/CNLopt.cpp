/*
 * CNLopt.cpp
 *
 *  Created on: Feb 13, 2015
 *      Author: fbaron
 */

 /*
 * Copyright (c) 2015 Fabien Baron
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

#include "CNLopt.h"
#include "CWorkerThread.h"
#include "CModelList.h"


CNLopt::CNLopt()
{
	mMinimizerID = "NLopt";
	mMinimizerName = "NLopt";
}

CNLopt::~CNLopt()
{

}

CMinimizerPtr CNLopt::CreateNELDERMEAD()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_NELDERMEAD;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateDIRECTL()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GN_DIRECT_L;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateDIRECT()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GN_DIRECT;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateCRS2()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GN_CRS2_LM; 
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateMLSLLDS()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_G_MLSL_LDS;
  nl->mAlgorithmSecondary = NLOPT_LN_NELDERMEAD;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateSTOGORAND()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GD_STOGO_RAND;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateISRES()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GN_ISRES;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateESCH()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_GN_ESCH;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateCOBYLA()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_COBYLA;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateBOBYQA()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_BOBYQA;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateNEWUOA()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_NEWUOA_BOUND;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreatePRAXIS()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_PRAXIS;
  return CMinimizerPtr(nl);
}

CMinimizerPtr CNLopt::CreateSBPLX()
{ 
  CNLopt* nl = new CNLopt(); 
  nl->mAlgorithm = NLOPT_LN_SBPLX;
  return CMinimizerPtr(nl);
}


double CNLopt::ErrorFunc(unsigned int nParams, const double* params, double* grad, void * misc)
{
	// Get the "this" pointer
        CNLopt * minimizer = reinterpret_cast<CNLopt*>(misc);
	minimizer->mEvals++;
	// See if we have been requested to exit
	if(!minimizer->mRun)
	{
	  nlopt_force_stop(minimizer->mOpt);
	}

	// Set the free parameters to the current nominal values determined by the minimizer
	CModelListPtr model_list = minimizer->mWorkerThread->GetModelList();
	model_list->SetFreeParameters(params, nParams, false);
	
	minimizer->mWorkerThread->GetChi(&minimizer->mChis[0], minimizer->mChis.size());
	return ComputeChi2r(minimizer->mChis, minimizer->mNParams);
}

string CNLopt::GetExitString(nlopt_result exit_num)
{
	string tmp;
	switch(exit_num)
	{

	case 1:
	  tmp =" NLOPT_SUCCESS = 1,  Generic success return value.";
	  break;
	case 2:
	  tmp ="NLOPT_STOPVAL_REACHED = 2, Optimization stopped because stopval (above) was reached.";
	  break;
	case 3:
	  tmp = "NLOPT_FTOL_REACHED = 3, Optimization stopped because ftol_rel or ftol_abs (above) was reached.";
	  break;
	case 4:
	  tmp = "NLOPT_XTOL_REACHED = 4, Optimization stopped because xtol_rel or xtol_abs (above) was reached.";
	  break;
	case 5:
	  tmp="NLOPT_MAXEVAL_REACHED = 5, Optimization stopped because maxeval (above) was reached.";
	  break;
	case 6:
	  tmp="NLOPT_MAXTIME_REACHED = 6, Optimization stopped because maxtime (above) was reached.";
	  break;
	case -1:
	  tmp="NLOPT_FAILURE = -1, Generic failure code.";
	  break;
	case -2:
	  tmp="NLOPT_INVALID_ARGS = -2, Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.).";
	  break;
	case -3:
	  tmp = "NLOPT_OUT_OF_MEMORY = -3, Ran out of memory.";
	  break;
	case -4: 
	  tmp="NLOPT_ROUNDOFF_LIMITED = -4, Halted because roundoff errors limited progress.";
	  break;
	case -5:
	  tmp="NLOPT_FORCED_STOP = -5, Halted because of a forced termination"; 
	  break;
	default:
		tmp =  "Unknown Exit Condition!";
		break;
	}

	return tmp;
}

/// Prints out NLopt results (from testmpfit.c)
void CNLopt::printresult(double * x, int n_pars, int n_data, vector<string> names, double minf, nlopt_result result)
{
	if ((x == NULL) || (n_pars == 0))
		return;

	double value = 0;
	double scale;
	string name = "";

	printf("Exit code: %s\n", GetExitString(result).c_str());
	printf("Lowest chi2 achieved: %f after %u chi2 evaluations\n", minf, mEvals);
	printf("Best-fit parameters:\n");
	for(int i=0; i < n_pars; i++)
	{
		printf("  P[%d] = %f (%s)\n", i, x[i], names[i].c_str());
	}

}

void CNLopt::run()
{
	// Run the minimizer using this instance of CMinimizer_NLopt
	run(&CNLopt::ErrorFunc);
	ExportResults();
}

int CNLopt::run(double (*error_func)(unsigned int nParams, const double* params, double* grad, void* misc))
{
	// Create a member function pointer

	unsigned int n_data = mWorkerThread->GetDataSize();
        mOpt = nlopt_create(mAlgorithm, mNParams);

	//	xopt = new double [mNParams];
	lb = new double [mNParams];
	ub = new double [mNParams];
	
	// Copy out the initial values for the parameters:
	CModelListPtr model_list = mWorkerThread->GetModelList();
	model_list->GetFreeParameters(mParams, mNParams, false);
	vector<string> names = model_list->GetFreeParamNames();
	vector< pair<double, double> > min_max = model_list->GetFreeParamMinMaxes();

	// Init parameter values
	for(int i = 0; i < mNParams; i++)
	{
		lb[i] = min_max[i].first;
		ub[i] = min_max[i].second;
		//	xopt = mParams[i];
	}

	nlopt_set_lower_bounds(mOpt,lb);
	nlopt_set_upper_bounds(mOpt,ub);
	
	nlopt_set_ftol_rel(mOpt, 1e-4); // stopping criterion = when chi2 changes by less than 0.1%
	nlopt_set_min_objective(mOpt, error_func, (void*)this);

	int major=0, minor=0, bugfix=0;
	nlopt_version(&major, &minor, &bugfix);
	printf("Starting NLopt version %d.%d.%d\n", major, minor, bugfix);
	printf("Algorithm = %s \n", nlopt_algorithm_name(mAlgorithm));


	if(mAlgorithm == NLOPT_G_MLSL_LDS) // set local optimizer to be used with a global search
	  {
	    printf("Secondary Algorithm = %s \n", nlopt_algorithm_name(mAlgorithmSecondary));
	    mOptSecondary = nlopt_create(mAlgorithmSecondary, mNParams);
	    nlopt_set_lower_bounds(mOptSecondary,lb);
	    nlopt_set_upper_bounds(mOptSecondary,ub);
	    nlopt_set_ftol_rel(mOptSecondary, 1e-4); // stopping criterion = when chi2 changes by less than 0.1%
	    nlopt_set_min_objective(mOptSecondary, error_func, (void*)this);
	    nlopt_set_local_optimizer(mOpt, mOptSecondary);
	  }

	mIsRunning = true;
	mEvals = 0;
	double minf;	
	// Call NLopt.  Note, the results are saved in mParams upon completion.	
	nlopt_result result = nlopt_optimize(mOpt, mParams, &minf);
	
	mIsRunning = false;
	printresult(mParams, mNParams, n_data, names, minf, result);

	delete(lb);
	delete(ub);

	nlopt_destroy(mOpt);
	if(mAlgorithm == NLOPT_G_MLSL_LDS) // also destroy local optimizer
	  {
	   nlopt_destroy(mOptSecondary);
	  }


	return mEvals;
}
