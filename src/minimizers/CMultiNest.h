/*
 * CMMultiNest.h
 *
 *  Created on: Jan 26, 2012
 *      Author: bkloppenborg
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

#ifndef CMULTINEST_H_
#define CMULTINEST_H_

#include "CMinimizer.h"
#include "multinest.h"

class CMultiNest: public CMinimizerThread
{

public:
	CMultiNest();
	virtual ~CMultiNest();

	static shared_ptr<CMinimizerThread> Create();

	static double ComputeLogZ(valarray<double> & residuals, const valarray<double> & uncertainties);

	static void dumper(int & nSamples, int &nlive, int &nPar, double **physLive, double **posterior, double **paramConstr, double &maxLogLike, double &logZ, double &logZerr, void * misc);

	static void log_likelihood(double * Cube, int & ndim, int & npars, double & lnew, void * misc);
	void run();

};

#endif /* CMULTINEST_H_ */
