/*
 * CWorkerList.h
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 */

#ifndef CWORKERLIST_H_
#define CWORKERLIST_H_

#include <vector>
#include <memory>

using namespace std;

class CWorker;
typedef shared_ptr<CWorker> CWorkerPtr;

class CWorkerList
{
protected:
	vector<CWorkerPtr> mWorkers;

public:
	virtual ~CWorkerList();
	CWorkerList();

	void InitCL();
	void InitGL();
};

#endif /* CWORKERLIST_H_ */
