/*
 * CMinimizer_mpfit.cpp
 *
 *  Created on: Feb 1, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_mpfit.h"
#include "CCL_GLThread.h"

CMinimizer_mpfit::CMinimizer_mpfit(CCL_GLThread * cl_gl_thread)
	: CMinimizer(cl_gl_thread)
{
	mResiduals = NULL;
}

CMinimizer_mpfit::~CMinimizer_mpfit()
{
	delete[] mResiduals;
}

/// The error function called by the minimzer.  Notice, this is a member function (non-static)
/// so we have to use boost::bind to access it from C/Fortran function calls.
int CMinimizer_mpfit::ErrorFunc(int nData, int nParams, double * params, double * deviates, double ** derivs, void * misc)
{
	CMinimizer_mpfit * tmp = reinterpret_cast<CMinimizer_mpfit*>(misc);
	int n_data_sets = 0;
	int n_data_alloc = 0;
	int n_data_offset = 0;

	// Convert the double parameter values back to floats
	printf("Parameters: ");
	for(int i = 0; i < nParams; i++)
	{
		tmp->mParams[i] = float(params[i]);
		printf("(%f %f) ", params[i], tmp->mParams[i]);
	}
	printf("\n");

	// Set the parameters:
	tmp->mCLThread->SetFreeParameters(tmp->mParams, nParams);

	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
	n_data_sets = tmp->mCLThread->GetNDataSets();
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		n_data_alloc = tmp->mCLThread->GetNDataAllocated(data_set);
		tmp->mCLThread->SetTime(tmp->mCLThread->GetDataAveJD(data_set));
		tmp->mCLThread->EnqueueOperation(GLT_RenderModels);
		tmp->mCLThread->GetChi(data_set, tmp->mResiduals + n_data_offset, n_data_alloc);
		n_data_offset += n_data_alloc;
	}


	// Copy the errors back into the double array:
//	printf("Residuals:\n");
	for(int i = 0; i < nData; i++)
	{
		deviates[i] = double(tmp->mResiduals[i]);
//		printf("%i %f \n", i, deviates[i]);
	}

	return 0;
}

void CMinimizer_mpfit::Init()
{
	CMinimizer::Init();
	int nData = mCLThread->GetNDataAllocated();

	mResiduals = new float[nData];
}

/// Prints out cmpfit results (from testmpfit.c)
void CMinimizer_mpfit::printresult(double * x, mp_result * result, vector<string> names, vector< pair<float, float> > min_max)
{
	if ((x == 0) || (result == 0))
		return;

	double value = 0;
	double err = 0;
	double scale;
	string name = "";

	printf("INIT CHI-SQUARE = %f \n", result->orignorm);
	printf("     CHI-SQUARE = %f    (%d DOF)\n", result->bestnorm, result->nfunc-result->nfree);
	printf("   R-CHI-SQUARE = %f\n", result->bestnorm / result->nfunc-result->nfree);
	printf("           NPAR = %d\n", result->npar);
	printf("          NFREE = %d\n", result->nfree);
	printf("        NPEGGED = %d\n", result->npegged);
	printf("          NITER = %d\n", result->niter);
	printf("           NFEV = %d\n", result->nfev);
	printf("\n");

	for(int i=0; i< result->npar; i++)
	{
		scale = min_max[i].second - min_max[i].first;
		value = x[i] * scale + min_max[i].first;
		err = result->xerror[i] * scale;
		name = names[i];
		printf("  P[%d] = %f +/- %f (%s)\n", i, value, err, name.c_str());
	}
}

int CMinimizer_mpfit::run()
{
	// Create a member function pointer
	int status = 0;
	int nParams = mCLThread->GetNFreeParameters();
	int nData = mCLThread->GetNDataAllocated();
	mp_result result;
	memset(&result,0,sizeof(result));
	result.xerror = new double[nParams];

	// Setup the input parameters and values:
	mp_par_struct * pars = new mp_par_struct[nParams];
	double * input_params = new double[nParams];
	float * tmp = new float[nParams];
	mCLThread->GetFreeParameters(tmp, nParams);
	vector<string> names = mCLThread->GetFreeParamNames();
	vector< pair<float, float> > min_max = mCLThread->GetFreeParamMinMaxes();

	// Init parameter values
	for(int i = 0; i < nParams; i++)
	{
		input_params[i] = double(tmp[i]);
		pars[i].fixed = 0;
		pars[i].limited[0] = 1;
		pars[i].limited[1] = 1;
		pars[i].limits[0] = 0;
		pars[i].limits[1] = 1;
		pars[i].step = 0.1;
		pars[i].relstep = 0.05;
		pars[i].side = 2;
		pars[i].deriv_debug = 0;
	}

	status = mpfit(&CMinimizer_mpfit::ErrorFunc, nData, nParams, input_params, pars, 0, (void*) this, &result);

	// Render the model with the best-fit parameters:
	for(int i = 0; i < nParams; i++)
		tmp[i] = float(input_params[i]);

	mCLThread->SetFreeParameters(tmp, nParams);
	mCLThread->EnqueueOperation(GLT_RenderModels);


	// Print out the exit code and fit information.
	printf("Minimizer Exit Code: %d\n", status);
	printresult(input_params, &result, names, min_max);

	delete[] result.xerror;
	delete[] input_params;
	delete[] pars;
	delete[] tmp;

	return 0;
}
