/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 */

#include "CMinimizer.h"
#include "CCL_GLThread.h"
#include "CMinimizer_Benchmark.h"
#include "CMinimizer_mpfit.h"
#include "CMinimizer_MultiNest.h"
#include "CMinimizer_levmar.h"

CMinimizer::CMinimizer(CCL_GLThread * cl_gl_thread)
{
	mCLThread = cl_gl_thread;
	mParams = NULL;
	mType = NONE;
}

CMinimizer::~CMinimizer()
{
	delete[] mParams;
}

CMinimizer * CMinimizer::GetMinimizer(CMinimizer::MinimizerTypes type, CCL_GLThread * cl_gl_thread)
{
	CMinimizer * tmp;
	switch(type)
	{
	case CMPFIT:
		tmp = new CMinimizer_mpfit(cl_gl_thread);
		break;

	case MULTINEST:
		tmp = new CMinimizer_MultiNest(cl_gl_thread);
		break;

	case LEVMAR:
		tmp = new CMinimizer_levmar(cl_gl_thread);
		break;

	default:
	case BENCHMARK:
		tmp = new CMinimizer_Benchmark(cl_gl_thread);
		break;
	}

	return tmp;
}

vector< pair<CMinimizer::MinimizerTypes, string> > CMinimizer::GetTypes(void)
{
	vector< pair<CMinimizer::MinimizerTypes, string> > tmp;
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::BENCHMARK, "Benchmark"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::CMPFIT, "CMPFit"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::MULTINEST, "MultiNest"));
	tmp.push_back(pair<CMinimizer::MinimizerTypes, string> (CMinimizer::LEVMAR, "Levmar"));

	return tmp;
}

void CMinimizer::Init()
{
	mParams = new float[mCLThread->GetNFreeParameters()];
}
