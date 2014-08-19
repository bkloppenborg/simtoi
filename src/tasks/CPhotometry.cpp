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
#include "minimizers/CBenchmark.h"

extern string EXE_FOLDER;

CPhotometry::CPhotometry(CWorkerThread * WorkerThread)
	: CTask(WorkerThread)
{
	mFBO_render = NULL;
	mFBO_storage = NULL;

	mLibOI = NULL;
	mLibOIInitialized = false;

	// Describe the data and provide extensions
	mDataDescription = "Photometric data";
	mExtensions.push_back("phot");
}

CPhotometry::~CPhotometry()
{
	delete mLibOI;

	if(mFBO_render) delete mFBO_render;
	if(mFBO_storage) delete mFBO_storage;
}

/// \brief Creates a new data set by selecting a subset of the loaded data
void CPhotometry::BootstrapNext(unsigned int maxBootstrapFailures)
{

}

CTaskPtr CPhotometry::Create(CWorkerThread * WorkerThread)
{
	return CTaskPtr(new CPhotometry(WorkerThread));
}

void CPhotometry::Export(string folder_name)
{
	InitBuffers();

	ofstream real_data;
	ofstream sim_data;
	ofstream summary;

	bool first_point = true;
	double sim_flux = 1;
	double sim_mag = 0;
	double t0_delta_mag = 0;
	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Open the statistics file in append mode:
	summary.open(folder_name + "summary.txt", ios::app | ios_base::in | ios_base::out);
	summary.precision(8);

	// Iterate through the data, copying the mag_err into the uncertainties buffer
	for(auto data_file: mData)
	{
		// Create some local storage for the chi elements we compute below.
		vector<double> chi_values;

		// Open the real data file:
		real_data.open(folder_name + data_file->base_filename + ".phot");
		real_data.width(15);
		real_data.precision(8);

		real_data << "# Input photometric data file as parsed by SIMTOI.." << endl;
		real_data << "# CSV format: JD, mag, mag_error " << endl;

		// Open the synthetic data file:
		first_point = true;
		sim_data.open(folder_name + data_file->base_filename + "_model.phot");
		sim_data.width(15);
		sim_data.precision(8);
		// Provide some information about the format
		sim_data << "# Simulated photometry from SIMTOI" << endl;
		sim_data << "# The data is normalized to the first data point in the real data." << endl;
		sim_data << "# CSV format: JD, mag " << endl;

		// Iterate over the data points in this data file:
		for(auto data_point: data_file->data)
		{
			// Write out to the real data file:
			real_data << data_point->jd << "," << data_point->mag << "," << data_point->mag_err << endl;

			// Simulate the photometry
			sim_mag = SimulatePhotometry(model_list, data_point->jd);
			// Cache the t = 0 magnitude.
			if(first_point)
			{
				t0_delta_mag = sim_mag - data_point->mag;
				first_point = false;
			}
			// Add the zero-point offset:
			sim_mag -= (t0_delta_mag);
			// Write out oto the simulated file
			sim_data << data_point->jd << "," << sim_mag << endl;

			// Append the chi (residual / error) for this data point:
			chi_values.push_back((sim_mag - data_point->mag) / data_point->mag_err);
		}
		// Close the file.
		real_data.close();
		sim_data.close();

		// Lastly write out some useful information to the summary file.
		// Here we compute the reduced chi2 for this data set:
		double chi2r = 0;
		for(auto chi: chi_values)
		{
			chi2r += chi * chi;
		}
		chi2r /= chi_values.size();

		// Write out the chi2r for this file:
		summary << data_file->base_filename << ".phot," << mDataDescription << "," << chi2r << endl;
	}

	// Close the statistics file.
	summary.close();
}

void CPhotometry::GetChi(double * chi, unsigned int size)
{
	InitBuffers();

	// Enable if you want to see frames per second
	//int total_start = CBenchmark::GetMilliCount();

	double sim_flux = 1;
	double sim_mag = 0;
	double t0_delta_mag = 0;
	unsigned int index = 0;
	CModelListPtr model_list = mWorkerThread->GetModelList();

	// Iterate through the data, copying the mag_err into the uncertainties buffer
	for(auto data_file: mData)
	{
		for(auto data_point: data_file->data)
		{
			sim_mag = SimulatePhotometry(model_list, data_point->jd);

			// Cache the t = 0 magnitude.
			if(index == 0)
				t0_delta_mag = sim_mag - data_point->mag;

			// Add the zero-point offset:
			sim_mag -= (t0_delta_mag);

			// store the residual calculation
			chi[index] = (sim_mag - data_point->mag) / data_point->mag_err;

			// increment the index
			index += 1;
		}
	}

	// Enable if you want to see frames per second
//	int total_time = CBenchmark::GetMilliSpan(total_start);
//	cout << "Photometric loop completed! It took: " << total_time << " ms." << endl;
//	cout << " Framerate (fps): " << double(size * 1000) / double(total_time) << endl;
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
		mLibOI->SetImageSource(mFBO_storage->handle(), LibOIEnums::OPENGL_TEXTUREBUFFER);
		mLibOI->SetImageInfo(width, height, depth, scale);

		// Get LibOI up and running
		mLibOI->Init();

		mLibOIInitialized = true;
	}
}

void CPhotometry::InitGL()
{
	if(mFBO_render) delete mFBO_render;
	if(mFBO_storage) delete mFBO_storage;

	mFBO_render = mWorkerThread->CreateMAARenderbuffer();
	mFBO_storage = mWorkerThread->CreateStorageBuffer();
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
	//	JD,mag,sig_mag,ANYTHING_ELSE

	string line;

	// Create a new data file for storing input data.
	CPhotometricDataFilePtr data_file = CPhotometricDataFilePtr(new CPhotometricDataFile());
	string base_filename = StripPath(filename);
	base_filename = StripExtension(base_filename, mExtensions);
	data_file->base_filename = base_filename;

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

double CPhotometry::SimulatePhotometry(CModelListPtr model_list, double jd)
{
	double sim_flux = 0;

	// Set the time, render the model
	model_list->SetTime(jd);
	mFBO_render->bind();
	model_list->Render(mWorkerThread->GetView());
	mFBO_render->release();

	// Blit to the storage buffer (for liboi to use the image)
	mWorkerThread->BlitToBuffer(mFBO_render, mFBO_storage);
	// Blit to the screen (to show the user, not required, but nice.
	mWorkerThread->BlitToScreen(mFBO_render);

	// Compute the flux:
	mLibOI->CopyImageToBuffer(0);

	// Get the simulated flux, convert it to a simulated magnitude using
	// -2.5 * log(counts)
	sim_flux = mLibOI->TotalFlux();
	return -2.5 * log10(sim_flux);
}
