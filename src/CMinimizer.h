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
protected:
	bool mIsRunning;

public:
	enum MinimizerTypes
	{
		NONE,
		BENCHMARK,
		MULTINEST,
		LEVMAR,
		TEST,
		LAST_VALUE	// this must always be the last value in this enum.
	};

public:
	CCL_GLThread * mCLThread;
	double * mParams;
	unsigned int mNParams;
	bool mRun;
	string mResultsBaseFilename;;

	CMinimizer::MinimizerTypes mType;

	CMinimizer(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer();

	void ExportResults(double * params, int n_params, bool no_setparams=false);

	static CMinimizer * GetMinimizer(CMinimizer::MinimizerTypes type, CCL_GLThread * cl_gl_thread);
	static vector< pair<CMinimizer::MinimizerTypes, string> > GetTypes(void);

	virtual void Init();
	bool IsRunning();

	virtual int run() = 0;
	void Stop();
};

#endif /* CMINIMIZER_H_ */
