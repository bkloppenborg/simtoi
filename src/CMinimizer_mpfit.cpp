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
	for(int i = 0; i < nData; i++)
		deviates[i] = double(tmp->mResiduals[i]);
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
	int nParams = mCLThread->GetNFreeParameters();
	int nData = mCLThread->GetNData();

	double * input_params = new double[nParams];

	for(int i = 0; i < nParams; i++)
		input_params[i] = 0.5;

	mpfit(&CMinimizer_mpfit::ErrorFunc, nData, nParams, input_params, 0, 0, (void*) this, NULL);

	delete[] input_params;

	return 0;
}
