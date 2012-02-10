/*
 * CMinimizer_Benchmark.h
 *
 *  Created on: Feb 10, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZER_BENCHMARK_H_
#define CMINIMIZER_BENCHMARK_H_

#include "CMinimizer.h"

class CMinimizer_Benchmark: public CMinimizer
{
public:
	CMinimizer_Benchmark(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_Benchmark();

	static int GetMilliCount();
	static int GetMilliSpan(int nTimeStart);

	int run();
};

#endif /* CMINIMIZER_BENCHMARK_H_ */
