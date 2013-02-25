/*
 * CTask.h
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

#ifndef CTASK_H_
#define CTASK_H_

#include <string>
#include <vector>
#include <valarray>

#include "CWorkerThread.h"

class CTask;
typedef shared_ptr<CTask> CTaskPtr;
class CWorkerThread;
typedef shared_ptr<CWorkerThread> CWorkerPtr;

using namespace std;

class CTask
{
protected:
	string mExeFolder;
	CWorkerThread * mWorkerThread;

	string mDataDescription; ///< A short few-word phrase to describe the data.
	vector<string> mExtensions; ///< A list of valid extensions for this task.

public:
	CTask(CWorkerThread * WorkerThread);
	virtual ~CTask();

	virtual void Export(string folder_name) = 0;

	virtual void GetChi(double * chis, unsigned int size) = 0;
	virtual string GetDataDescription();
	virtual vector<string> GetExtensions();
	virtual unsigned int GetNData() = 0;
	virtual void GetUncertainties(double * residuals, unsigned int size) = 0;

	virtual void InitGL() {};
	virtual void InitCL() {};

	virtual void OpenData(string filename) = 0;

	static string StripPath(string filename);
	static string StripExtension(string filename, vector<string> & valid_extensions);

};

#endif /* CTASK_H_ */
