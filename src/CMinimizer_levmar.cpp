/*
 * CMinimizer_levmar.cpp
 *
 *  Created on: Feb 13, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_levmar.h"
#include "levmar.h"
#include "CCL_GLThread.h"

CMinimizer_levmar::CMinimizer_levmar(CCL_GLThread * cl_gl_thread)
: CMinimizer(cl_gl_thread)
{
	mType = CMinimizer::LEVMAR;
	mResiduals = NULL;
}

CMinimizer_levmar::~CMinimizer_levmar()
{
	delete[] mResiduals;
}

void CMinimizer_levmar::ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc)
{
	// Get the "this" pointer
	CMinimizer_levmar * minimizer = reinterpret_cast<CMinimizer_levmar*>(misc);
	int n_data_sets = minimizer->mCLThread->GetNDataSets();
	int n_data_alloc = 0;
	int n_data_offset = 0;

	// Set the parameters (note, they are already scaled)
	minimizer->mCLThread->SetFreeParameters(params, nParams, false);

	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		n_data_alloc = minimizer->mCLThread->GetNDataAllocated(data_set);
		minimizer->mCLThread->SetTime(minimizer->mCLThread->GetDataAveJD(data_set));
		minimizer->mCLThread->EnqueueOperation(GLT_RenderModels);
		minimizer->mCLThread->GetChi(data_set, minimizer->mResiduals + n_data_offset, n_data_alloc);
		n_data_offset += n_data_alloc;
	}

	// Copy the errors back into the double array:
//	printf("Residuals:\n");
	for(int i = 0; i < nOutput; i++)
	{
		output[i] = double(minimizer->mResiduals[i]);
//		printf("%i %f \n", i, deviates[i]);
	}
}

void CMinimizer_levmar::Init()
{
	CMinimizer::Init();
	int nData = mCLThread->GetNDataAllocated();

	mResiduals = new float[nData];
}

/// Prints out cmpfit results (from testmpfit.c)
void CMinimizer_levmar::printresult(double * x, int n_pars, int n_data, vector<string> names, double * info)
{
	if ((x == 0) || (n_pars == 0))
		return;

	double value = 0;
	double err = 0;
	double scale;
	string name = "";

	printf("Reason for exiting: %i\n", int(info[6]));

	for(int i=0; i < n_pars; i++)
		printf("  P[%d] = %f +/- %f (%s)\n", i, x[i], err, names[i].c_str());

	if(int(info[6]) == 7)
	{
		printf("Dumping Residuals Buffer:\n");
		for(int i = 0; i < n_data; i++)
			printf(" %i %f\n", i, mResiduals[i]);
	}

}

int CMinimizer_levmar::run()
{
	// Create a member function pointer
	int iterations = 0;
	int max_iterations = 1000;
	int nParams = mCLThread->GetNFreeParameters();
	int nData = mCLThread->GetNDataAllocated();
	double x[nData];
	double lb[nParams];
	double ub[nParams];
	double info[LM_INFO_SZ];
	double opts[LM_OPTS_SZ];

	// Setup the options (LM_* from levmar.h):
	// info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
	//  opts[0] = |e||_2 				= LM_INIT_MU = 1E-03
	//  opts[1] = ||J^T e||_inf 		= LM_STOP_THRESH = 1E-15;
	//  opts[2] = ||Dp||_2 				= LM_STOP_THRESH = 1E-15;
	//  opts[3] = mu/max[J^T J]_ii ] 	= LM_STOP_THRESH * LM_STOP_THRESH = 1E-17 * 1E-17;
	//  opts[4]= LM_DIFF_DELTA;
	opts[0]= 1E-2;
	opts[1]= 1E-8;
	opts[2]= 1E-6;
	opts[3]= 1E-6;
	opts[4]= 1E-6;

	// Copy out the initial values for the parameters:
	double params[nParams];
	mCLThread->GetFreeParameters(params, nParams, true);
	vector<string> names = mCLThread->GetFreeParamNames();
	vector< pair<double, double> > min_max = mCLThread->GetFreeParamMinMaxes();

	// Init parameter values
	for(int i = 0; i < nParams; i++)
	{
		lb[i] = min_max[i].first;
		ub[i] = min_max[i].second;
	}

	// Call levmar:
	iterations = dlevmar_bc_dif(&CMinimizer_levmar::ErrorFunc, params, x, nParams, nData, lb, ub, opts, max_iterations, NULL, info, NULL, NULL, (void*)this);

	mCLThread->SetFreeParameters(params, nParams, false);
	mCLThread->EnqueueOperation(GLT_RenderModels);

	printf("Levmar executed %i iterations.\n", iterations);
	printresult(params, nParams, nData, names, info);

	return 0;
}
