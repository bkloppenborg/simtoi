 /*
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
 *
 * Copyright (c) 2013 Brian Kloppenborg
 */

#ifndef CTASKLIST_H_
#define CTASKLIST_H_

#include <vector>
#include <memory>
#include <valarray>
#include <map>

using namespace std;

class CDataInfo;

class CTask;
typedef shared_ptr<CTask> CTaskPtr;

class CWorkerThread;
typedef shared_ptr<CWorkerThread> CWorkerPtr;

class CTaskList
{
protected:
	vector<CTaskPtr> mTasks;

public:
	CTaskList(CWorkerThread * WorkerThread);
	virtual ~CTaskList();

	void BootstrapNext(unsigned int maxBootstrapFailures);

	void clearData();

	void Export(string export_folder);

	void GetChi(double * chis, unsigned int size);
	unsigned int GetDataSize();
	vector<string> GetFileFilters();
	int GetNDataFiles();
	CTaskPtr getTask(unsigned int i) { return mTasks[i]; };
	void GetUncertainties(double * uncertainties, unsigned int size);

	CDataInfo OpenData(string filename);

	void InitCL();
	void InitGL();

	void RemoveData(unsigned int data_index);

	unsigned int size() { return mTasks.size(); };
};

#endif /* CTASKLIST_H_ */
