/*
 * CMinimizer_Verify.h
 *
 *  Created on: Feb 15, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_VERIFY_H_
#define CMINIMIZER_VERIFY_H_

#include "CMinimizer.h"

class CMinimizer_Verify: public CMinimizer
{
public:
	CMinimizer_Verify(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_Verify();

	int run();
};

#endif /* CMINIMIZER_VERIFY_H_ */
