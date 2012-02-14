/*
 * CMinimizer.h
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 *
 *  A generic base class for minimization routines.
 *
 *  NOTE: This class also maintains a static function for minimizer creation.  If you add a new
 *  minimizer, be sure to modify GetTypes() and GetMinimizer()
 *
 */

#ifndef CMINIMIZER_H_
#define CMINIMIZER_H_

#include <string>
#include <cstdio>
#include <utility>
#include <vector>

using namespace std;

class CCL_GLThread;

class CMinimizer
{
public:
	enum MinimizerTypes
	{
		NONE,
		BENCHMARK,
		MULTINEST,
		LEVMAR,
		LAST_VALUE	// this must always be the last value in this enum.
	};

public:
	CCL_GLThread * mCLThread;
	float * mParams;

	CMinimizer::MinimizerTypes mType;

	CMinimizer(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer();

	static CMinimizer * GetMinimizer(CMinimizer::MinimizerTypes type, CCL_GLThread * cl_gl_thread);
	static vector< pair<CMinimizer::MinimizerTypes, string> > GetTypes(void);

	virtual void Init();
	virtual int run() = 0;
};

// If the minimizer cannot pass a void* pointer to the minimization function do a
//   minimizer_tmp::minimizer = reinterpret_cast<void*>(this)
// in the run function before calling the actual minimizer.  NOTE: This limits the
// number of concurrent minimizer using void * minimizer to at most ONE instance.
namespace minimizer_tmp
{
	static void * minimizer;
}

#endif /* CMINIMIZER_H_ */
