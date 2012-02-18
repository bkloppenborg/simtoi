/*
 * CMinimizer_Verify.cpp
 *
 *  Created on: Feb 15, 2012
 *      Author: bkloppenborg
 */

#include "CMinimizer_Verify.h"
#include "CCL_GLThread.h"

CMinimizer_Verify::CMinimizer_Verify(CCL_GLThread * cl_gl_thread)
: CMinimizer(cl_gl_thread)
{
	mType = VERIFY;

}

CMinimizer_Verify::~CMinimizer_Verify()
{
	// TODO Auto-generated destructor stub
}

/// Run the Verification functions
int CMinimizer_Verify::run()
{
	mCLThread->EnqueueOperation(GLT_RenderModels);
	mCLThread->EnqueueOperation(CLT_Tests);
	return 0;
}
