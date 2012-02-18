/*
 * CMinimizer_Test.cpp
 *
 *  Created on: Feb 15, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_Test.h"
#include "CCL_GLThread.h"

CMinimizer_Test::CMinimizer_Test(CCL_GLThread * cl_gl_thread)
: CMinimizer(cl_gl_thread)
{
	mType = TEST;

}

CMinimizer_Test::~CMinimizer_Test()
{
	// TODO Auto-generated destructor stub
}

/// Run the Verification functions
int CMinimizer_Test::run()
{
	mCLThread->EnqueueOperation(GLT_RenderModels);
	mCLThread->EnqueueOperation(CLT_Tests);
	return 0;
}
