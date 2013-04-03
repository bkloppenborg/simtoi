/*
 * CBoostrap_Levmar.h
 *
 *  Created on: Mar 29, 2013
 *      Author: bkloppenborg
 */

#ifndef CBOOTSTRAP_LEVMAR_H_
#define CBOOTSTRAP_LEVMAR_H_

#include "CMinimizerThread.h"

class CBootstrap_Levmar: public CMinimizerThread
{
public:
	CBootstrap_Levmar();
	virtual ~CBootstrap_Levmar();

	static CMinimizerPtr Create();

protected:
	unsigned int mBootstrapFailures;
	unsigned int mMaxBootstrapFailures;
	unsigned int mIterations;

	ofstream mOutputFile;

	CMinimizerPtr mLevmar;

public:
	virtual void ExportResults();
	void Init(shared_ptr<CWorkerThread> worker_thread);

	virtual void run();

	virtual void stop();
};

#endif /* CBOOTSTRAP_LEVMAR_H_ */
