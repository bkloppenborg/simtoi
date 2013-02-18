/*
 * COI.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
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

#include "COI.h"
#include <GL/gl.h>
#include <GL/glu.h>

extern string EXE_FOLDER;

COI::COI(CWorkerPtr WorkerThread)
	: CTask(WorkerThread)
{

}

COI::~COI()
{
	delete mLibOI;

	glDeleteFramebuffers(1, &mFBO);
	glDeleteFramebuffers(1, &mFBO_texture);
	glDeleteFramebuffers(1, &mFBO_depth);
	glDeleteFramebuffers(1, &mFBO_storage);
	glDeleteFramebuffers(1, &mFBO_storage_texture);
}

CTaskPtr COI::Create(CWorkerPtr WorkerThread)
{
	return CTaskPtr(new COI(WorkerThread));
}

string COI::GetDataDescription()
{
	return "OIFITS data";
}

vector<string> COI::GetDataTypes()
{
	vector<string> temp;
	temp.push_back("*.oifits");
	temp.push_back("*.fits");
	return temp;
}

unsigned int COI::GetNData()
{
	mLibOI->GetNData();
}

void COI::GetResiduals(valarray<double> & residuals)
{

}

void COI::GetUncertainties(valarray<double> & uncertainties)
{

}

void COI::InitGL()
{
	// Init framebuffers
	mWorkerThread->CreateGLBuffer(mFBO, mFBO_texture, mFBO_depth, mFBO_storage, mFBO_storage_texture);
}

void COI::InitCL()
{
	// First initialize local storage:
	unsigned int width = mWorkerThread->GetImageWidth();
	unsigned int height = mWorkerThread->GetImageHeight();
	double scale = mWorkerThread->GetImageScale();

	// Initialize liboi using the worker thread's OpenCL+OpenGL context
//	mLibOI = new CLibOI()
//	mLibOI->SetImageSource(mFBO_storage_texture, LibOIEnums::OPENGL_TEXTUREBUFFER);
//	mLibOI->SetKernelSourcePath(EXE_FOLDER + "/kernels/");
}

void COI::LoadData(string filename)
{
	mLibOI->LoadData(filename);
}

void COI::OpenData(string filename)
{

}
