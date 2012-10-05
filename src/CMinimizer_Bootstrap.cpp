/*
 * CMinimizer_Bootstrap.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: bkloppen
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

#include "CMinimizer_Bootstrap.h"

#include <fstream>
#include <sstream>

#include "CCL_GLThread.h"

using namespace std;


CMinimizer_Bootstrap::CMinimizer_Bootstrap(CCL_GLThread * cl_gl_thread)
: CMinimizer(cl_gl_thread)
{
	mType = CMinimizer::BOOTSTRAP;
}

CMinimizer_Bootstrap::~CMinimizer_Bootstrap()
{
	// Stop the minimizer and call it's destructor.
	if(mMinimizer != NULL)
		mMinimizer->Stop();

	delete mMinimizer;
}

void CMinimizer_Bootstrap::ExportResults(double * params, int n_params, bool no_setparams)
{
	stringstream filename;
	ofstream outfile;
	double tmp[3];

	// Open the statistics file for writing:
	filename.str("");
	filename << mResultsBaseFilename << "_bootstrap.txt";
	outfile.open(filename.str().c_str());
	outfile.width(15);
	outfile.precision(8);
	outfile << "# Param1 Param2 ... ParamN Chi2" << endl;

	// write the data to the file
	vector< vector<double> >::iterator row;
	vector<double>::iterator cell;
	int nEntries = mResults.size();
	// iterate over each row
	for (row = mResults.begin() ; row < mResults.end(); row++)
	{
		// iterate over each cell
		for(cell = row->begin(); cell < row->end(); cell++)
			outfile << *cell;

		outfile << endl;
	}

	outfile.close();
}

void CMinimizer_Bootstrap::Init()
{
	CMinimizer::Init();
}

int CMinimizer_Bootstrap::run()
{
	// init local storage
	vector<double> tmp_vec;
	double tmp_chi2 = 0;
	int nBootstrap = 1000;

	for(int i = 0; i < nBootstrap; i++)
	{
		if(!mRun)
			break;


		// Save the results.  Start by copying the current entry to the temporary vector:
		mMinimizer->GetResults(mParams, mNParams);
		tmp_vec.clear();
		for(int i = 0; i < mNParams; i++)
			tmp_vec.push_back(mParams[i]);
		// append the chi2
		tmp_vec.push_back(tmp_chi2);

		// push this vector onto the back of mResults
		mResults.push_back(tmp_vec);
	}

	// Save the results of the bootstrapper.
    ExportResults(mParams, mNParams, true);

	return 0;
}

/// Tells the thread to gracefully exit.
/// The event loop of the minimization thread must mRun for this routine to work correctly.
void CMinimizer_Bootstrap::Stop()
{
	mRun = false;
	// Tell the corresponding minimizer to stop
	if(mMinimizer != NULL)
		mMinimizer->Stop();
}

