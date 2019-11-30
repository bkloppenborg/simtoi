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
 * Copyright (c) 2013 Brian Kloppenborg
 */

#include "COI.h"

#include "OpenGL.h" // OpenGL includes, plus several workarounds for various OSes

#include <stdexcept>
#include <fstream>
#include <random>
#include "oi_tools.hpp"
// TODO: Figure out how to pull in additional calibrator models
#include "CUniformDisk.h"
#include "CTask.h"

#include "CModelList.h"

extern string EXE_FOLDER;

COI::COI(CWorkerThread * WorkerThread)
	: CTask(WorkerThread)
{
	mNV2 = 0;
	mNT3 = 0;

	mFBO_render = NULL;
	mFBO_storage = NULL;

	mLibOI = NULL;
	mLibOIInitialized = false;
	mInteropEnabled = false;

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

	if(mFBO_render) delete mFBO_render;
	if(mFBO_storage) delete mFBO_storage;
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
//	pair<double, double> old_cal_prop(0.419 * MAS_TO_RAD, 0.063 * MAS_TO_RAD);		// eta Aur MIRC calibrator size
//	pair<double, double> new_cal_prop(0.453 * MAS_TO_RAD, 0.012 * MAS_TO_RAD);		// eta Aur from Maestro et al 2013.

//	std::default_random_engine generator;
//	std::normal_distribution<double> new_cal_distribution(new_cal_prop.first, new_cal_prop.second);

	// Setup the old calibrator
//	OICalibratorPtr old_cal = OICalibratorPtr( new ccoifits::CUniformDisk(old_cal_prop.first) );
//	OICalibratorPtr new_cal = OICalibratorPtr( new ccoifits::CUniformDisk(new_cal_distribution(generator)) );

	// Recalibrate the data.
	// TODO: Note this function assumes the SAME calibrator is used on ALL data sets. This probably isn't true.
	// This issue is listed in https://github.com/bkloppenborg/simtoi/issues/53.
	for(int i = 0; i < mData.size(); i++)
	{
		auto data = mData[i];

		// Recalibrate, bootstrap, then push to the OpenCL device:
//		OIDataList t_data = Recalibrate(data, old_cal, new_cal);
//		t_data = Bootstrap_Spectral(t_data);
		OIDataList t_data = Bootstrap_Spectral(data);

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

void COI::clearData()
{
	unsigned int n_data_sets = mLibOI->GetNDataSets();

	for(int i = n_data_sets - 1; i > -1; i--)
	{
		mLibOI->RemoveData(i);
	}
}

void  COI::copyImage()
{
	// Intel integrated GPUs do not have cl_khr_gl_sharing on Linux, I don't know
	// about AMD. Thus we explicitly copy the data from OpenGL to a host-side
	// buffer, then copy the data back to the GPU. A total waste of resources
	// but the only workaround which is reasonable at the present time.
	if(mInteropEnabled)
	{
		mLibOI->CopyImageToBuffer(0);
	}
	else
	{
		unsigned int width = mWorkerThread->GetImageWidth();
		unsigned int height = mWorkerThread->GetImageHeight();
		// something with mFBO_storage

		GLint buffer_format = this->mWorkerThread->glPixelDataFormat();
		switch(buffer_format)
		{
		case GL_FLOAT:
			// floating point buffer, simply copy the data directly
			glReadPixels(0, 0, width, height, GL_RED, buffer_format, mHostImage);
			mLibOI->CopyImageToBuffer(0);

			break;

		case GL_UNSIGNED_INT:
			// unsigned integer buffer, copy and convert the data
			unsigned int * temp = new unsigned int[width * height];
			glReadPixels(0, 0, width, height, GL_RED, buffer_format, temp);

			float sum = 0;
			for(int i = 0; i < width * height; i++)
			{
				mHostImage[i] = float(temp[i]);
			}

			delete[] temp;
			break;

			mLibOI->CopyImageToBuffer(0);
		}

	}
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
		model_list->SetWavelength(mLibOI->GetDataAveWavelength(data_set));
		// render
		mFBO_render->bind();
		model_list->Render(mWorkerThread->GetView());
		mFBO_render->release();

		// Blit to the storage buffer (for liboi to use the image)
		mWorkerThread->BlitToBuffer(mFBO_render, mFBO_storage);
		mWorkerThread->BlitToScreen(mFBO_render);
		copyImage();

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
		// Clear out the chi buffer:
		for(unsigned int i = 0; i < max_data; i++)
			temp_chi[i] = 0;
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
		model_list->SetWavelength(mLibOI->GetDataAveWavelength(data_set));
		mFBO_render->bind();
		model_list->Render(mWorkerThread->GetView());
		mFBO_render->release();

		// Blit to the storage buffer (for liboi to use the image)
		mWorkerThread->BlitToBuffer(mFBO_render, mFBO_storage);
		// Blit to the screen (to show the user, not required, but nice.
		mWorkerThread->BlitToScreen(mFBO_render);

		copyImage();

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

/// Kludge implementation of getDataInfo. Always reports on the last opened file.
CDataInfo COI::getDataInfo()
{
	stringstream temp;

	CDataInfo info;
	info.setFilename(mFilenameShort);

	temp << "V2: " << mNV2 << " T3: " << mNT3;
	info.setDescription(temp.str());

//	info.setJDMin(mJDStart);
//	info.setJDMax(mJDEnd);
	info.setJDMean(mJDMean);
	info.setWavelengthMean(mWavelengthMean);

	return info;
}

unsigned int COI::GetNData()
{
	return mLibOI->GetNData();
}

int COI::GetNDataFiles()
{
	int n_data = 0;
	if(mLibOI != NULL)
		n_data = mLibOI->GetNDataSets();

	return n_data;
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

		if(mInteropEnabled)
			mLibOI->SetImageSource(mFBO_storage->handle(), LibOIEnums::OPENGL_TEXTUREBUFFER);
		else
		{
			mHostImage = new float[width * height];
			mLibOI->SetImageSource(mHostImage);
		}

		mLibOI->SetImageInfo(width, height, depth, scale);

		// Get LibOI up and running
		mLibOI->Init();

		mLibOIInitialized = true;
	}
}

void COI::InitGL()
{
	if(mFBO_render) delete mFBO_render;
	if(mFBO_storage) delete mFBO_storage;

	mFBO_render = mWorkerThread->CreateMAARenderbuffer();
	mFBO_storage = mWorkerThread->CreateStorageBuffer();
}

void COI::InitCL()
{
	// Initialize liboi using the worker thread's OpenCL+OpenGL context
	COpenCLPtr OCL = mWorkerThread->GetOpenCL();
	mLibOI = new CLibOI(OCL);
	// detect if we have an integrated GPU
	mInteropEnabled = mLibOI->isInteropEnabled();

	if(!mInteropEnabled)
		cout << "Warning: Your device does not support OpenCL-OpenGL interoperability, this will result in a significant performance degredation.";
}

CDataInfo COI::OpenData(string filename)
{
	mFilename = filename;
	mFilenameShort = StripPath(filename);
//	mFilenameNoExtension = StripExtension(mFilenameShort, mExtensions);

	unsigned int data_id = mLibOI->LoadData(filename);
	mNV2 = mLibOI->GetNV2(data_id);
	mNT3 = mLibOI->GetNT3(data_id);
	mJDMean = mLibOI->GetDataAveJD(data_id);
	mWavelengthMean = mLibOI->GetDataAveWavelength(data_id);

	return getDataInfo();
}

void COI::RemoveData(unsigned int data_index)
{
	if(mLibOI != NULL)
	{
		mLibOI->RemoveData(data_index);
	}
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
