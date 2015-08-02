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
	unsigned int mNV2;
	unsigned int mNT3;

protected:
	QGLFramebufferObject * mFBO_render;
	QGLFramebufferObject * mFBO_storage;

	CLibOI * mLibOI;
	bool mLibOIInitialized;

	float * mTempFloat;

	bool mInteropEnabled;
	GLfloat * mHostImage;

	vector<OIDataList> mData;	/// A copy of the original data. Used when bootstrapping

public:
	COI(CWorkerThread * WorkerThread);
	virtual ~COI();

	virtual void BootstrapNext(unsigned int maxBootstrapFailures);

	static CTaskPtr Create(CWorkerThread * worker);
	void clearData();
	void copyImage();

	void Export(string folder_name);

	virtual void GetChi(double * residuals, unsigned int size);
	virtual CDataInfo getDataInfo();
	virtual unsigned int GetNData();
	virtual int GetNDataFiles();
	virtual void GetUncertainties(double * residuals, unsigned int size);

	void InitBuffers();
	virtual void InitGL();
	virtual void InitCL();

	CDataInfo OpenData(string filename);

	void RemoveData(unsigned int data_index);

	double sum(vector<float> & values, unsigned int start, unsigned int end);
};

#endif /* COI_H_ */
