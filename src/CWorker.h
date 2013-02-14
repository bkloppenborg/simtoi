/*
 * CWorker.h
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 *
 *  A pure virtual class dictating the functions workers must implement.
 */

 /*
 * Copyright (c) 2013 Brian Kloppenborg
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

#ifndef CWORKER_H_
#define CWORKER_H_

#include <string>
#include <vector>
#include <valarray>

class CCL_GLThread;

using namespace std;

class CWorker
{
protected:
	string mExeFolder;
	CCL_GLThread * mWorkerThread;

public:
	CWorker(CCL_GLThread * WorkerThread);
	virtual ~CWorker();

	virtual string GetDataDescription() = 0;
	virtual vector<string> GetDataTypes() = 0;
	virtual unsigned int GetNData() = 0;
	virtual void GetResiduals(valarray<double> & residuals) = 0;
	virtual void GetUncertainties(valarray<double> & uncertainties) = 0;

	virtual void LoadData(string filename) = 0;

};

#endif /* CWORKER_H_ */
