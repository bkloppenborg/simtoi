/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 */
 
 /*
 * Copyright (c) 2012 Brian Kloppenborg
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

#include "CMinimizerThread.h"
#include "CWorkerThread.h"
#include "CModelList.h"
#include <QDir>

using namespace std;

CMinimizerThread::CMinimizerThread()
{
	mParams = NULL;
	mNParams = 0;
	mRun = false;
	mIsRunning = false;
	mSaveDirectory = "/tmp/model";
	mMinimizerName = "";
	mMinimizerID = "";
}

CMinimizerThread::~CMinimizerThread()
{
	// Cleanly stop the thread
	stop();

	// Clear up memory
	delete[] mParams;
}

double CMinimizerThread::ComputeChi2r(valarray<double> & chis, unsigned int n_params)
{
	// Square the numerator, divide by the uncertainties
	chis *= chis;

	double chi2_sum = chis.sum();

	// Now compute the sum dividied by (n_data - n_params - 1)
	return chi2_sum / (chis.size() - n_params - 1);
}

/// \brief Exports the parameter names, best-fit values, and model data.
///
/// Exports minimization results using the best-fit parameters stored in
/// the mParams buffer.
void CMinimizerThread::ExportResults()
{
	ofstream outfile;

	// Get a pointer to the model list;
    CModelListPtr model_list = mWorkerThread->GetModelList();
	vector<string> names = model_list->GetFreeParamNames();

	OpenStatisticsFile(outfile);

	// Write out the save file header, including the parameter names:
	WriteHeader(names, outfile);

	vector<double> best_fit;
	for(int i = 0; i < mNParams; i++)
		best_fit.push_back(mParams[i]);

	WriteRow(best_fit, outfile);
	outfile.close();

	// Set the best-fit parameter values
	model_list->SetFreeParameters(mParams, mNParams, false);

	// Tell the worker thread to save files.
	QString save_folder = QString::fromStdString(mSaveDirectory);
	mWorkerThread->ExportResults(save_folder + "/");
}

/// \brief Returns the unique ID assigned to this minimizer.
string CMinimizerThread::GetID()
{
	return mMinimizerID;
}

/// \brief Returns the best-fit parameters after minimization has completed.
///
/// Once the minimization is complete the minimizer is *required* to set the `mParams`
/// variable to the best-fit minimization parameters. This function can be used
/// to copy the first `n_params` values into the `results` buffer.
///
/// \param results A pre-allocated buffer of size `n_params` to which results are written.
/// \param n_params The size of the `results` buffer.
void CMinimizerThread::GetResults(double * results, int n_params)
{
	// Copy the values
	for(int i = 0; i < mNParams && i < n_params; i++)
		results[i] = mParams[i];
}

/// \brief Initialization function.
///
/// \param worker_thread A shared pointer to the worker thread
void CMinimizerThread::Init(shared_ptr<CWorkerThread> worker_thread)
{
	mWorkerThread = worker_thread;

	CModelListPtr model_list = mWorkerThread->GetModelList();
	mNParams = model_list->GetNFreeParameters();
	mParams = new double[mNParams];
	mRun = true;

	// Allocate storage for the residuals and uncertainties
	unsigned int n_data = mWorkerThread->GetDataSize();
	mChis = valarray<double>(n_data);
	mUncertainties = valarray<double>(n_data);
}

/// \brief Opens the statistics file and sets the properties for the ofstream
///
/// \param file An uninitialized ofstream for the statistics files
void CMinimizerThread::OpenStatisticsFile(ofstream & file)
{
	stringstream filename;

	if(file.is_open())
		file.close();

	// Open the statistics file for writing:
	filename.str("");
	filename << mSaveDirectory << "/best_fit.txt";
	file.open(filename.str().c_str());
	file.width(15);
	file.precision(8);
}


/// \brief Changes the default directory to which the minimizer saves files.
///
/// Some minimization engines may write out intermediate files during the
/// minimization process. This function permits you to set an absolute path
/// and filename prefix which is used by the minimizers.
///
/// \param filename An absolute path prefix for minimizer files.
void CMinimizerThread::SetSaveDirectory(string folder_name)
{
	if(folder_name.size() > 0)
		mSaveDirectory = folder_name;
}

/// \brief Stops the minimizer.
void CMinimizerThread::stop()
{
	mRun = false;
}

/// \brief Writes a row of data to the output file.
void CMinimizerThread::WriteRow(double * data, unsigned int size, double chi2r, ofstream & output)
{
	for(int i = 0; i < size; i++)
		output << data[i] << ", ";

	output << chi2r << endl;
}

/// \brief Writes the parameter names
void CMinimizerThread::WriteHeader(vector<string> & param_names, ofstream & outfile)
{
	outfile << "# Parameter names in a column." << endl;
	outfile << "# Param0, ..., ParamN" << endl;
	WriteRow(param_names, outfile);
}
