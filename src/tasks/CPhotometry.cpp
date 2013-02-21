/*
 * CPhotometry.cpp
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

#include "CPhotometry.h"
#include "textio.hpp"
#include "CModelList.h"

extern string EXE_FOLDER;

CPhotometry::CPhotometry(CWorkerThread * WorkerThread)
	: CTask(WorkerThread)
{
    mFBO = 0;
	mFBO_texture = 0;
	mFBO_depth = 0;
    mFBO_storage = 0;
	mFBO_storage_texture = 0;

	mLibOI = NULL;
	mLibOIInitialized = false;
}

CPhotometry::~CPhotometry()
{
	delete mLibOI;

	if(mFBO) glDeleteFramebuffers(1, &mFBO);
	if(mFBO_texture) glDeleteFramebuffers(1, &mFBO_texture);
	if(mFBO_depth) glDeleteFramebuffers(1, &mFBO_depth);
	if(mFBO_storage) glDeleteFramebuffers(1, &mFBO_storage);
	if(mFBO_storage_texture) glDeleteFramebuffers(1, &mFBO_storage_texture);
}

void CPhotometry::Export(string folder_name)
{

}

CTaskPtr CPhotometry::Create(CWorkerThread * WorkerThread)
{
	return CTaskPtr(new CPhotometry(WorkerThread));
}

string CPhotometry::GetDataDescription()
{
	return "Photometric data";
}

vector<string> CPhotometry::GetExtensions()
{
	vector<string> temp;
	temp.push_back("phot");
	return temp;
}

unsigned int CPhotometry::GetNData()
{
	unsigned int n_data = 0;
	for(auto datafile: mData)
	{
		n_data += datafile->GetNData();
	}

	return n_data;
}

void CPhotometry::GetResiduals(double * residuals, unsigned int size)
{
	InitBuffers();

	double sim_flux, sim_mag;
	double sim_mag_t0, data_mag_t0;
	unsigned int index = 0;
	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Iterate through the data, copying the mag_err into the uncertainties buffer
	for(auto data_file: mData)
	{
		for(auto data_point: data_file->data)
		{
			// Set the time, render the model
			model_list->SetTime(data_point->jd);
			model_list->Render(mFBO, mWorkerThread->GetImageWidth(), mWorkerThread->GetImageHeight());

			// Blit to the storage buffer (for liboi to use the image)
			mWorkerThread->BlitToBuffer(mFBO, mFBO_storage);
			// Blit to the screen (to show the user, not required, but nice.
			mWorkerThread->BlitToScreen(mFBO);

			// Compute the flux:
			mLibOI->CopyImageToBuffer(0);

			// Get the simulated flux, convert it to a simulated magnitude using
			// -2.5 * log(counts)
			sim_flux = mLibOI->TotalFlux(true);
			sim_mag = -2.5 * log(sim_flux);

			// Cache the t = 0 magnitude.
			if(index == 0)
			{
				sim_mag_t0 = sim_mag;
				data_mag_t0 = data_point->mag;
			}

			// store the residual calculation
			residuals[index] = (sim_mag - data_point->mag) - (sim_mag_t0 - data_mag_t0);

			// increment the index
			index += 1;
		}
	}

}

void CPhotometry::GetUncertainties(double * uncertainties, unsigned int size)
{
	InitBuffers();

	unsigned int index = 0;

	// Iterate through the data, copying the mag_err into the uncertainties buffer
	for(auto data_file: mData)
	{
		for(auto data_point: data_file->data)
		{
			// be sure not to go out of bounds
			if(index < size)
				uncertainties[index] = data_point->mag_err;
			else
				break;

			// increment the index
			index += 1;
		}
	}
}


void CPhotometry::InitBuffers()
{
	// During the first call there may be some remaining initialization to be done.
	// Lets make sure they are ready to go:
	if(!mLibOIInitialized)
	{
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

void CPhotometry::InitGL()
{
	// Init framebuffers
	mWorkerThread->CreateGLBuffer(mFBO, mFBO_texture, mFBO_depth, mFBO_storage, mFBO_storage_texture);
}

void CPhotometry::InitCL()
{
	// Initialize liboi using the worker thread's OpenCL+OpenGL context
	mLibOI = new CLibOI(mWorkerThread->GetOpenCL());
}

void CPhotometry::OpenData(string filename)
{
	// The photometric data files must conform to a very specific format
	// foremost they MUST have the same extension as returned from
	// CPhotometry::GetExtensions()
	// The file must conform to the following format:
	//	# comment lines prefixed by #, /, ;, or !
	//	JD mag sig_mag

	string line;

	// Create a new data file for storing input data.
	CPhotometricDataFilePtr data_file = CPhotometricDataFilePtr(new CPhotometricDataFile());

	// Get a vector of the non-comment lines in the text file:
	vector<string> lines = ReadFile(filename, "#/;!", "Could not read photometric data file " + filename + ".");
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		line = lines[i];
		line = StripWhitespace(line);
		vector<string> t_line = SplitString(line, ',');

		if(t_line.size() < 3)
		{
			cout << "WARNING: Could not parse '" + line + "' from photometric data file " + filename << endl;
			continue;
		}

		// Attempt to cast variables
		try
		{
			CPhotometricDataPointPtr t_data = CPhotometricDataPointPtr(new CPhotometricDataPoint());
			t_data->jd = atof(t_line[0].c_str());
			t_data->mag = atof(t_line[1].c_str());
			t_data->mag_err = atof(t_line[2].c_str());

			data_file->data.push_back(t_data);
		}
		catch(...)
		{
			cout << "WARNING: Could not parse '" + line + "' from photometric data file " + filename << endl;
		}
	}

	// The data was imported correctly, push it onto our data list
	mData.push_back(data_file);
}
