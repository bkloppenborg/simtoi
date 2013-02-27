/*
 * CGridSearch.h
 *
 *  Created on: Oct 4, 2012
 *      Author: bkloppen
 *
 *  Implementation of a basic grid-search minimizer.
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

#ifndef CGRIDSEARCH_H_
#define CGRIDSEARCH_H_

#include "CMinimizerThread.h"

class CGridSearch: public CMinimizerThread
{
protected:
	vector< pair<double, double> > mMinMax;
	vector<double> mSteps;
	ofstream mOutputFile;

	valarray<double> mBestFit;

public:
	CGridSearch();
	virtual ~CGridSearch();

	static CMinimizerPtr Create();

	void ExportResults();

	void GridSearch(unsigned int level);

	virtual void Init(shared_ptr<CWorkerThread> worker_thread);

	void run();

	void WriteRow(double * data, unsigned int size, double chi2r, ofstream & output);
};

#endif /* CMINIMIZER_GRIDSEARCH_H_ */
