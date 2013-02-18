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

	void Export(string export_folder);

	void GetResiduals(valarray<double> & residuals);
	void GetUncertainties(valarray<double> & uncertainties);

	void OpenData(string filename);

	void InitCL();
	void InitGL();
};

#endif /* CTASKLIST_H_ */
