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
 *  Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CBENCHMARK_H_
#define CBENCHMARK_H_

#include "CMinimizerThread.h"

class CBenchmark: public CMinimizerThread
{
public:
	CBenchmark();
	virtual ~CBenchmark();

	static CMinimizerPtr Create();

	virtual void ExportResults();

	static int GetMilliCount();
	static int GetMilliSpan(int nTimeStart);

	void run();
};

#endif /* CBENCHMARK_H_ */
