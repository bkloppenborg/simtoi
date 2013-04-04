/*
 * CPhotometry.h
 *
 *  Created on: Feb 21, 2013
 *      Author: bkloppen
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

#ifndef CPHOTOMETRY_H_
#define CPHOTOMETRY_H_

#include "CTask.h"
#include "liboi.hpp"

#include <string>

using namespace std;
using namespace liboi;

class CPhotometricDataPoint
{
public:
	double jd;
	double mag;
	double mag_err;
};
typedef shared_ptr<CPhotometricDataPoint> CPhotometricDataPointPtr;

class CPhotometricDataFile
{
public:
	string base_filename; ///< The filename less the path and extension.
	double wavelength;
	vector<CPhotometricDataPointPtr> data;

	unsigned int GetNData() { return data.size(); };
};
typedef shared_ptr<CPhotometricDataFile> CPhotometricDataFilePtr;

class CPhotometry: public CTask
{
protected:
    GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;
    GLuint mFBO_storage;
	GLuint mFBO_storage_texture;

	CLibOI * mLibOI;
	bool mLibOIInitialized;

	vector<CPhotometricDataFilePtr> mData;

public:
	CPhotometry(CWorkerThread * WorkerThread);
	virtual ~CPhotometry();

	virtual void BootstrapNext(unsigned int maxBootstrapFailures);

	static CTaskPtr Create(CWorkerThread * worker);

	virtual void Export(string folder_name);

	virtual void GetChi(double * residuals, unsigned int size);
	virtual unsigned int GetNData();
	virtual void GetUncertainties(double * residuals, unsigned int size);

	void InitBuffers();
	virtual void InitGL();
	virtual void InitCL();

	virtual void OpenData(string filename);

	double SimulatePhotometry(CModelListPtr, double jd);
};

#endif /* CPHOTOMETRY_H_ */
