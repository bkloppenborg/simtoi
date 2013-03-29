/*
 * CBoostrap_Levmar.h
 *
 *  Created on: Mar 29, 2013
 *      Author: bkloppenborg
 */

#ifndef CBOOTSTRAP_LEVMAR_H_
#define CBOOTSTRAP_LEVMAR_H_

#include "CLevmar.h"

class CBootstrap_Levmar: public CLevmar
{
public:
	CBootstrap_Levmar();
	virtual ~CBootstrap_Levmar();

protected:
	unsigned int mBootstrapFailures;
	unsigned int mMaxBootstrapFailures;
	unsigned int mIterations;

	ofstream mOutputFile;

public:
	virtual void ExportResults();
	virtual void run();
};

#endif /* CBOOTSTRAP_LEVMAR_H_ */
