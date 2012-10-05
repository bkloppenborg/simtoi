/*
 * CMinimizer_Bootstrap.h
 *
 *  Created on: Oct 4, 2012
 *      Author: bkloppen
 *
 *  A basic bootstrap minimizer that selects a subset of n-data, possibly
 *  including repeats, from the original data of size n and runs a
 *  minimization engine.  The results of this procedure are saved to
 *  a file upon completion.
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

#include "CMinimizer.h"

class CMinimizer_Bootstrap: public CMinimizer {
public:
	CMinimizer_Bootstrap(CCL_GLThread * cl_gl_thread);
	virtual ~CMinimizer_Bootstrap();

	CMinimizer * mMinimizer;

	vector< vector<double> > mResults;

	virtual void ExportResults(double * params, int n_params, bool no_setparams);

	void Init();
	int run();
	virtual void Stop();
};

#endif /* CMINIMIZER_BOOTSTRAP_H_ */
