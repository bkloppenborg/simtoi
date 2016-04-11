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
 *  Copyright (c) 2013 Brian Kloppenborg
 */

#ifndef CBOOTSTRAP_LEVMAR_H_
#define CBOOTSTRAP_LEVMAR_H_

#include "CMinimizerThread.h"

class CLevmar;

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

	shared_ptr<CLevmar> mLevmar;

public:
	void Init(shared_ptr<CWorkerThread> worker_thread);

	virtual void run();

	virtual void stop();
};

#endif /* CBOOTSTRAP_LEVMAR_H_ */
