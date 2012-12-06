/*
 * CMinimizer.h
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 *
 *  A generic base class for minimization routines.  All minimizers should have the following
 *  behavior:
 *  	1) If mRun == false, they should terminate gracefully, deallocating resources as needed
 *  	2) Upon completion, the best-fit parameters should be stored in mParams
 *  	3) Upon completion, results should be exported to a file.
 *
 *  NOTE: This class also maintains a static function for minimizer creation.  If you add a new
 *  minimizer, be sure to modify GetTypes() and GetMinimizer()
 *
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CMINIMIZER_H_
#define CMINIMIZER_H_

#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
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
		NONE = 0,
		BENCHMARK = 1,
		LEVMAR = 2,
		MULTINEST = 3,
		GRIDSEARCH = 4,
		BOOTSTRAP = 5,
		LAST_VALUE	// this must always be the last value in this enum.
	};

public:
	CCL_GLThread * mCLThread;
	double * mParams;	// Current parameter values. Set to best-fit parameters upon exit (required)
	unsigned int mNParams;
	bool mRun;
	string mResultsBaseFilename;;

	CMinimizer::MinimizerTypes mType;

	CMinimizer(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer();

	virtual void ExportResults(double * params, int n_params, bool no_setparams=false);

	static CMinimizer * GetMinimizer(CMinimizer::MinimizerTypes type, CCL_GLThread * cl_gl_thread);
	virtual void GetResults(double * results, int n_params);
	static vector< pair<CMinimizer::MinimizerTypes, string> > GetTypes(void);

	virtual void Init();
	bool IsRunning();

	virtual int run() = 0;
	virtual void Stop();

	template <class T>
	void WriteTable(vector< vector<T> > & data, ofstream & output)
	{
		// write the data to the file
		typename vector< vector<T> >::iterator row;
		typename vector<T>::iterator cell;
		int nEntries = data.size();
		// iterate over each row
		for (row = data.begin() ; row < data.end(); row++)
			WriteRow(*row, output);
	}

	template <class T>
	void WriteRow(vector<T> & data, ofstream & output)
	{
		// write the data to the file
		typename vector<T>::iterator column;
		int nEntries = data.size();
		// iterate over each row
		for (column = data.begin() ; column < data.end(); column++)
			output << *column << " ";;

		output << endl;
	}
};

#endif /* CMINIMIZER_H_ */
