/*
 * Clevmar.h
 *
 *  Created on: Feb 13, 2015
 *      Author: fbaron
 */
 
 /* 
 * Copyright (c) 2015 Fabien Baron
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

#ifndef CNLOPT_H_
#define CNLOPT_H_

using namespace std;

#include "nlopt.h"
#include "CMinimizerThread.h"

class CNLopt: public CMinimizerThread
{

public:
	CNLopt();
	virtual ~CNLopt();
	
	unsigned int mEvals;
	double* lb;
	double* ub;
	//	double* xopt;
	nlopt_opt mOpt; // hack to store nlopt
	nlopt_algorithm mAlgorithm; // algorithm used for minimization

	static CMinimizerPtr Create();

	static double ErrorFunc(unsigned int nParams, const double* params, double* grad, void * misc);
	string GetExitString(nlopt_result exit_num);

	void printresult(double * x, int n_pars, int n_data, vector<string> names, double minf, nlopt_result result);

	virtual void run();

	int run(double (*error_func)(unsigned int nParams, const double* params, double* grad, void* misc));

};

#endif /* CMINIMIZER_NLOPT_H_ */
