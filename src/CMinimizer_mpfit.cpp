/*
 * CMinimizer_mpfit.cpp
 *
 *  Created on: Feb 1, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_mpfit.h"
#include "CCL_GLThread.h"
#include "mpfit.h"

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

	// Convert the double parameter values back to floats
	printf("Parameters: ");
	for(int i = 0; i < nParams; i++)
	{
		tmp->mParams[i] = float(params[i]);
		printf("(%f %f) ", params[i], tmp->mParams[i]);
	}
	printf("\n");

	// Set the parameters:
	tmp->mCLThread->SetParameters(tmp->mParams, nParams);
	tmp->mCLThread->EnqueueOperation(GLT_RenderModels);

	// Now iterate through the data and pull out the residuals
	// TODO: Make this work with multiple data sets, probably will have to do pointer math.
	tmp->mCLThread->GetChi(0, tmp->mResiduals, nData);

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
	int nData = mCLThread->GetNData();

	mResiduals = new float[nData];
}

int CMinimizer_mpfit::run()
{
	// Create a member function pointer
	int status = 0;
	int nParams = mCLThread->GetNFreeParameters();
	int nData = mCLThread->GetNData();

	double * input_params = new double[nParams];
	mp_par_struct * pars = new mp_par_struct[nParams];

	for(int i = 0; i < nParams; i++)
	{
		input_params[i] = 0.9;
		pars[i].fixed = 0;
		pars[i].limited[0] = 0;
		pars[i].limited[1] = 0;
		pars[i].limits[0] = 0;
		pars[i].limits[1] = 0;
		pars[i].step = 0.01;
		pars[i].relstep = 0.001;
		pars[i].side = 2;
		pars[i].deriv_debug = 0;
	}


	status = mpfit(&CMinimizer_mpfit::ErrorFunc, nData, nParams, input_params, pars, 0, (void*) this, NULL);

	printf("Minimizer Exit Code: %d\n", status);

	delete[] input_params;
	delete[] pars;

	return 0;
}
