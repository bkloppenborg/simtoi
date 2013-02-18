/*
 * COI.h
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 *
 *  A worker class for Optical Interferometric data.
 *  Most of the hard work is outsourced to liboi
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

#ifndef COI_H_
#define COI_H_

#include "CTask.h"
#include "liboi.hpp"

using namespace liboi;

class COI: public CTask
{
protected:
    GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;
    GLuint mFBO_storage;
	GLuint mFBO_storage_texture;

	CLibOI * mLibOI;
	bool mLibOIInitialized;

	GLsizei mSamples;
	valarray<float> mTempFloat;

public:
	COI(CWorkerPtr WorkerThread);
	virtual ~COI();

	static CTaskPtr Create(CWorkerPtr worker);

	virtual string GetDataDescription();
	virtual vector<string> GetDataTypes();
	virtual unsigned int GetNData();
	virtual void GetResiduals(valarray<double> & residuals);
	virtual void GetUncertainties(valarray<double> & uncertainties);

	virtual void InitGL();
	virtual void InitCL();

	void OpenData(string filename);
};

#endif /* COI_H_ */
