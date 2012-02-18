/*
 * CMinimizer_Test.h
 *
 *  Created on: Feb 15, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_TEST_H_
#define CMINIMIZER_TEST_H_

#include "CMinimizer.h"

class CMinimizer_Test: public CMinimizer
{
public:
	CMinimizer_Test(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_Test();

	int run();
};

#endif /* CMINIMIZER_TEST_H_ */
