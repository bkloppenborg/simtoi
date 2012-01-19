/*
 * CSIMTOI.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: bkloppenborg
 */

#include "CSIMTOI.h"
#include "CGLThread.h"
#include "CLibOI.h"
// TODO: Perhaps we can figure out how to remove the model headers from here?
#include "CModelList.h"
#include "CModelSphere.h"

CSIMTOI::CSIMTOI()
{
	// Init the class members with some bogus values:
	mGL = NULL; //COpenGLThread::GetInstance();
	mCL = NULL;
	mModelList = NULL;
	mWindow_width = 1;
	mWindow_height = 1;
	mScale = 1;
	mShaderSourceDir = "";
	mKernelSourceDir = "";
}

CSIMTOI::~CSIMTOI()
{
	// Free memory.
	delete mCL;
	delete mModelList;
}

/// Instance of the class.
CSIMTOI CSIMTOI::TheInstance;

float CSIMTOI::GetChi2(int data_num, int wl_num)
{
	mCL->CopyImageToBuffer(wl_num);
	return mCL->ImageToChi2(data_num);
}

int CSIMTOI::GetNFreeParameters()
{
	return mModelList->GetNFreeParameters();
}

float CSIMTOI::GetLogLike(int data_num, int wl_num)
{
//	mCL->CopyImageToBuffer(wl_num);
//	mCL->ImageToLogLike(data_num);

	return 0;
}

/// Gets the current model parameters, scaled to physical units.
/// Returns the first 'size' of them in params.
void CSIMTOI::GetParameters(float * params, int size)
{
	mModelList->GetParameters(params, size);
}

/// Initialize SIMTOI
void CSIMTOI::Init(int argc, char *argv[])
{
	// Create the OpenCL Object, initialize
	mCL = new CLibOI();
	mCL->SetKernelSourcePath(mKernelSourceDir);
	mCL->Init(CL_DEVICE_TYPE_GPU, mWindow_width, mWindow_height, 1, mScale);
	//mCL->RegisterImage_GLTB(mGL->GetFramebufferTexture());

	// Load OIFITS data into memory
	mCL->LoadData("/home/bkloppenborg/workspace/simtoi/bin/epsaur.oifits");

	// Now init memory and routines
	mCL->InitMemory();
	mCL->InitRoutines();
}
