/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 */

#include "CMinimizer.h"
#include "CCL_GLThread.h"

CMinimizer::CMinimizer(CCL_GLThread * cl_gl_thread)
{
	mCLThread = cl_gl_thread;
}

CMinimizer::~CMinimizer()
{
	delete[] mParams;
}

void CMinimizer::Init()
{
	mParams = new float[mCLThread->GetNFreeParameters()];
}
