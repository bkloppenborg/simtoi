/*
 * CTaskList.h
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
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

	void Export(string export_folder);

	void GetChi(double * chis, unsigned int size);
	unsigned int GetDataSize();
	vector<string> GetFileFilters();
	CTaskPtr getTask(unsigned int i) { return mTasks[i]; };
	void GetUncertainties(double * uncertainties, unsigned int size);

	CDataInfo OpenData(string filename);

	void InitCL();
	void InitGL();

	unsigned int size() { return mTasks.size(); };
};

#endif /* CTASKLIST_H_ */
