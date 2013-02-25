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

#include "CModelList.h"

extern string EXE_FOLDER;

COI::COI(CWorkerThread * WorkerThread)
	: CTask(WorkerThread)
{
    mFBO = 0;
	mFBO_texture = 0;
	mFBO_depth = 0;
    mFBO_storage = 0;
	mFBO_storage_texture = 0;

	mLibOI = NULL;
	mLibOIInitialized = false;

	mTempFloat = NULL;

	// Describe the data and provide extensions
	mDataDescription = "OIFITS data";
	mExtensions.push_back("fit");
	mExtensions.push_back("fits");
	mExtensions.push_back("oifits");
}

COI::~COI()
{
	delete mLibOI;
	if(mTempFloat) delete mTempFloat;

	if(mFBO) glDeleteFramebuffers(1, &mFBO);
	if(mFBO_texture) glDeleteFramebuffers(1, &mFBO_texture);
	if(mFBO_depth) glDeleteFramebuffers(1, &mFBO_depth);
	if(mFBO_storage) glDeleteFramebuffers(1, &mFBO_storage);
	if(mFBO_storage_texture) glDeleteFramebuffers(1, &mFBO_storage_texture);
}

CTaskPtr COI::Create(CWorkerThread * WorkerThread)
{
	return CTaskPtr(new COI(WorkerThread));
}

void COI::Export(string folder_name)
{
	InitBuffers();

	CModelListPtr model_list = mWorkerThread->GetModelList();
	string filename = "";

	unsigned int n_data_sets = mLibOI->GetNDataSets();
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		// Get the base name for the file:
		filename = mLibOI->GetDataFileName(data_set);
		filename = StripPath(filename);
		filename = StripExtension(filename, mExtensions);

		// Set the current JD, render the model.
		model_list->SetTime(mLibOI->GetDataAveJD(data_set));
		model_list->Render(mFBO, mWorkerThread->GetImageWidth(), mWorkerThread->GetImageHeight());
		// Blit to the storage buffer (for liboi to use the image)
		mWorkerThread->BlitToBuffer(mFBO, mFBO_storage);
		mWorkerThread->BlitToScreen(mFBO);
		mLibOI->CopyImageToBuffer(0);

		// Now export the image, overwriting any image that already exists:
		mLibOI->ExportImage("!" + folder_name + filename + "_model.fits");

		// Now generate and save the simulated data:
		mLibOI->ImageToData(data_set);
		mLibOI->ExportData(data_set, folder_name + filename);
	}
}

void COI::GetChi(double * chis, unsigned int size)
{
	InitBuffers();

	unsigned int n_data_offset = 0;
	unsigned int n_data_alloc = 0;

	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
	unsigned int n_data_sets = mLibOI->GetNDataSets();
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		n_data_alloc = mLibOI->GetNDataAllocated(data_set);
		model_list->SetTime(mLibOI->GetDataAveJD(data_set));
		model_list->Render(mFBO, mWorkerThread->GetImageWidth(), mWorkerThread->GetImageHeight());

		// Blit to the storage buffer (for liboi to use the image)
		mWorkerThread->BlitToBuffer(mFBO, mFBO_storage);
		// Blit to the screen (to show the user, not required, but nice.
		mWorkerThread->BlitToScreen(mFBO);

		mLibOI->CopyImageToBuffer(0);

		// Notice, the ImageToChi expects a floating point array, not a valarray<float>.
		// C++11 guarantees that storage is contiguous so we can do pointer math
		// within the valarray storage without issues.
		mLibOI->ImageToChi(data_set, mTempFloat + n_data_offset, n_data_alloc);

		// Advance the pointer
		n_data_offset += n_data_alloc;
	}

	// Copy the floats from liboi into doubles for SIMTOI.
	for(unsigned int i = 0; i < size; i++)
	{
		chis[i] = double(mTempFloat[i]);
	}
}

unsigned int COI::GetNData()
{
	mLibOI->GetNData();
}

void COI::GetUncertainties(double * uncertainties, unsigned int size)
{
	InitBuffers();

	unsigned int n_data_offset = 0;
	unsigned int n_data_alloc = 0;

	// Now iterate through the data and pull out the residuals, notice we do pointer math on mResiduals
	unsigned int n_data_sets = mLibOI->GetNDataSets();
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		n_data_alloc = mLibOI->GetNDataAllocated(data_set);

		// Notice, the ImageToChi expects a floating point array, not a valarray<float>.
		// C++11 guarantees that storage is contiguous so we can do pointer math
		// within the valarray storage without issues.
		mLibOI->GetDataUncertainties(data_set, mTempFloat + n_data_offset, n_data_alloc);

		// Advance the pointer
		n_data_offset += n_data_alloc;
	}

	// Copy the floats from liboi into doubles for SIMTOI.
	for(unsigned int i = 0; i < size; i++)
	{
		uncertainties[i] = double(mTempFloat[i]);
	}
}

void COI::InitBuffers()
{
	// During the first call there may be some remaining initialization to be done.
	// Lets make sure they are ready to go:
	if(!mLibOIInitialized)
	{
		// Make sure the residuals buffer is large enough
		mTempFloat = new float[mLibOI->GetNDataAllocated()];

		// Get image properties
		unsigned int width = mWorkerThread->GetImageWidth();
		unsigned int height = mWorkerThread->GetImageHeight();
		unsigned int depth = mWorkerThread->GetImageDepth();
		double scale = mWorkerThread->GetImageScale();

		// Initalize remaining OpenCL items.
		mLibOI->SetKernelSourcePath(EXE_FOLDER + "/kernels/");
		mLibOI->SetImageSource(mFBO_storage_texture, LibOIEnums::OPENGL_TEXTUREBUFFER);
		mLibOI->SetImageInfo(width, height, depth, scale);

		// Get LibOI up and running
		mLibOI->Init();

		mLibOIInitialized = true;
	}
}

void COI::InitGL()
{
	// Init framebuffers
	mWorkerThread->CreateGLBuffer(mFBO, mFBO_texture, mFBO_depth, mFBO_storage, mFBO_storage_texture);
}

void COI::InitCL()
{
	// Initialize liboi using the worker thread's OpenCL+OpenGL context
	mLibOI = new CLibOI(mWorkerThread->GetOpenCL());
}

void COI::OpenData(string filename)
{
	mLibOI->LoadData(filename);
}
