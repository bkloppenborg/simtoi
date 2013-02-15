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

using namespace std;

class CTask;
typedef shared_ptr<CTask> CWorkerPtr;

class CTaskList
{
protected:
	vector<CWorkerPtr> mWorkers;

public:
	virtual ~CTaskList();
	CTaskList();

	void InitCL();
	void InitGL();
};

#endif /* CTASKLIST_H_ */
