/*
 * CMinimizer_Bootstrap.h
 *
 *  Created on: Oct 4, 2012
 *      Author: bkloppen
 *
 *  A re-implementation of the levmar minimizer that includes bootstrapping.
 *  This minimizer runs levmar like normal, but applies a mask to the chi
 *  which weights (potentially ignores) some chi elements. This, in effect
 *  selects a random subset of the data.
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

#ifndef CMINIMIZER_BOOTSTRAP_H_
#define CMINIMIZER_BOOTSTRAP_H_

#include "CMinimizer_levmar.h"
#include "oi_file.hpp"

using namespace ccoifits;

class CMinimizer_Bootstrap: public CMinimizer_levmar
{
protected:
	unsigned int mBootstrapFailures;
	unsigned int mMaxBootstrapFailures;

public:
	vector<OIDataList> mData;	// A copy of the original data

public:
	CMinimizer_Bootstrap();
	virtual ~CMinimizer_Bootstrap();

	vector< vector<double> > mResults;

	static CMinimizerPtr Create();

	static void ErrorFunc(double * params, double * output, int nParams, int nOutput, void * misc);
	virtual void ExportResults(double * params, int n_params, bool no_setparams);

	virtual void Init(shared_ptr<CCL_GLThread> cl_gl_thread);

	void Next();

	int run();
};

#endif /* CMINIMIZER_BOOTSTRAP_H_ */
