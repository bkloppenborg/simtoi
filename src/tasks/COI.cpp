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
#include <stdexcept>
#include <fstream>
#include "oi_tools.hpp"
// TODO: Figure out how to pull in additional calibrator models
#include "CUniformDisk.h"

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

/// \brief Creates a new data set via. bootstrapping and replacing currently loaded data.
void COI::BootstrapNext(unsigned int maxBootstrapFailures)
{
	unsigned int nBootstrapFailures;

	// If this is our first bootstrap, copy the OIDataList into local memory.
	if(mData.size() == 0)
	{
		int nData = mLibOI->GetNDataSets();
		for(int data_set = 0; data_set < nData; data_set++)
			mData.push_back(mLibOI->GetData(data_set));
	}

	// TODO: Calibrator information is hard-coded for eps Aur. This should be read in from elsewhere.
	pair<double, double> old_cal_prop(0.419 * MAS_TO_RAD, 0.063 * MAS_TO_RAD);		// eta Aur MIRC calibrator size
	pair<double, double> new_cal_prop(0.453 * MAS_TO_RAD, 0.012 * MAS_TO_RAD);		// eta Aur from Maestro et al 2013.

	std::default_random_engine generator;
	std::normal_distribution<double> new_cal_distribution(new_cal_prop.first, new_cal_prop.second);

	// Setup the old calibrator
	OICalibratorPtr old_cal = OICalibratorPtr( new ccoifits::CUniformDisk(old_cal_prop.first) );
	OICalibratorPtr new_cal = OICalibratorPtr( new ccoifits::CUniformDisk(new_cal_distribution(generator)) );

	// Recalibrate the data.
	// TODO: Note this function assumes the SAME calibrator is used on ALL data sets. This probably isn't true.
	// This issue is listed in https://github.com/bkloppenborg/simtoi/issues/53.
	for(int i = 0; i < mData.size(); i++)
	{
		auto data = mData[i];

		// Recalibrate, bootstrap, then push to the OpenCL device:
		OIDataList t_data = Recalibrate(data, old_cal, new_cal);
		t_data = Bootstrap_Spectral(t_data);

		// Replace the 0th entry with temp.
		// Due to a bug in ccoifits the total data size may not be preserved, so we need to
		// catch and repeat.
		try
		{
			mLibOI->ReplaceData(i, t_data);
		}
		catch(length_error& l)
		{
			// Generate an error message on stderr.
			cerr << " Warning: " << l.what() << " " << "Generating a new bootstrapped data set." << endl;
			nBootstrapFailures += 1;
			i--;

			// If we haven't exceeded the maximum number of bootstrap failures, repeat
			// this iteration.
			if(nBootstrapFailures < maxBootstrapFailures)
				continue;
			else
				throw runtime_error("Too many bootstrap data generation failures.");
		}
		nBootstrapFailures = 0;
	}
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

	// Open the summary/statistics file in append mode:
	ofstream summary;
	summary.open(folder_name + "summary.txt", ios::app | ios_base::in | ios_base::out);
	summary.precision(8);
	// Allocate a buffer in which the data may be stored.
	unsigned int max_data = mLibOI->GetMaxDataSize();
	vector<float> temp_chi(max_data);
	unsigned int n_vis = 0, n_vis2 = 0, n_t3 = 0, start = 0, n_data_size = 0;
	double total_chi2 = 0, vis_real_chi2 = 0, vis_imag_chi2 = 0, vis2_chi2 = 0, t3_real_chi2 = 0, t3_imag_chi2 = 0;

	unsigned int n_data_sets = mLibOI->GetNDataSets();
	for(int data_set = 0; data_set < n_data_sets; data_set++)
	{
		// Get the base name for the file:
		filename = mLibOI->GetDataFileName(data_set);
		filename = StripPath(filename);
		filename = StripExtension(filename, mExtensions);

		// Set the current JD, render the model.
		model_list->SetTime(mLibOI->GetDataAveJD(data_set));
		model_list->Render(mFBO, mWorkerThread->GetView());
		// Blit to the storage buffer (for liboi to use the image)
		mWorkerThread->BlitToBuffer(mFBO, mFBO_storage);
		mWorkerThread->BlitToScreen(mFBO);
		mLibOI->CopyImageToBuffer(0);

		// Now export the image, overwriting any image that already exists:
		mLibOI->ExportImage("!" + folder_name + filename + "_model.fits");

		// Now generate and save the simulated data:
		mLibOI->ImageToData(data_set);
		mLibOI->ExportData(data_set, folder_name + filename);

		// Generate the chi2
//		n_vis = mLibOI->GetNVis(data_set);
		n_vis = 0;
		n_vis2 = mLibOI->GetNV2(data_set);
		n_t3 = mLibOI->GetNT3(data_set);
		n_data_size = mLibOI->GetNDataAllocated(data_set);
		mLibOI->ImageToChi(data_set, &temp_chi[0], n_data_size);
		// Compute the chi-squared for each element.
		for(unsigned int i = 0; i < n_data_size; i++)
			temp_chi[i] *= temp_chi[i];

		// LibOI packages the interferometric data like this:
		//  [vis_real, vis_imag, vis2, t3_real, t3_imag]
		// where each type of data is of size:
		//  [n_vis, n_vis, n_vis2, n_t3, n_t3]
		start = 0;
		total_chi2 = sum(temp_chi, 0, n_data_size) / n_data_size;
		vis_real_chi2 = sum(temp_chi, start, n_vis) / n_vis;
		start += n_vis;
		vis_imag_chi2 = sum(temp_chi, start, n_vis) / n_vis;
		start += n_vis;
		vis2_chi2 = sum(temp_chi, start, n_vis2) / n_vis2;
		start += n_vis2;
		t3_real_chi2 = sum(temp_chi, start, n_t3) / n_t3;
		start += n_t3;
		t3_imag_chi2 = sum(temp_chi, start, n_t3) / n_t3;

		// Write out the results to the summary file
		summary << filename << ".oifits," << mDataDescription << ",";
		summary << total_chi2 << "," << n_data_size << ",";
		summary << vis_real_chi2 << "," << n_vis << ",";
		summary << vis_imag_chi2 << "," << n_vis << ",";
		summary << vis2_chi2 << "," << n_vis2 << ",";
		summary << t3_real_chi2 << "," << n_t3 << ",";
		summary << t3_imag_chi2 << "," << n_t3;
		summary << endl;
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
		model_list->Render(mFBO, mWorkerThread->GetView());

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

double COI::sum(vector<float> & values, unsigned int start, unsigned int end)
{
	double temp = 0;
	for(unsigned int i = start; i < start + end; i++)
	{
		temp += values[i];
	}

	return temp;
}
