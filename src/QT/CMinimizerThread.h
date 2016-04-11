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
 * Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CMINIMIZER_H_
#define CMINIMIZER_H_

#include <QThread>

#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <memory>
#include <valarray>

using namespace std;

class CWorkerThread;
class CMinimizerThread;
typedef shared_ptr<CMinimizerThread> CMinimizerPtr;

/// \brief A generic base class for minimization routines.
///
/// All minimization routines used by SIMTOI inherit from this class.
/// SIMTOI expects the following behavior from all minimizers:
///  - To be used with SIMTOI, all minimizers MUST:
///		-# Have a unique ID stored in `mMinimizerID` (see the list of built-in IDs)
///		-# Implement a static function to create an instance of the
///		   minimizer wrapped in a CMinimizerPtr. (i.e. see
///		   `CMinimizer_GridSearch::Create()`).
///		-# Periodically check mRun and, if false, terminate gracefully
///		   deallocating any self-allocated resources (i.e. no calls to `exit`)
///		-# Upon completion, the best-fit parameters MUST be stored in mParams
///  - Upon completion, the results (with applicable uncertainty estimates) SHOULD
/// 	be exported to a savefile using mSaveFileBasename as the save prefix.
///
/// Before a minimizer is started, SIMTOI will call `Init()` to setup several
/// base-class datamembers. If you overwride this function, be sure to call
/// the base-class routine.
class CMinimizerThread : public QThread
{
	Q_OBJECT

protected:
	bool mIsRunning;		///< Indicates if the thread is running

public:
	shared_ptr<CWorkerThread> mWorkerThread; ///< Pointer to the worker thread.
	double * mParams;		///< Current parameter values. Set to best-fit parameters upon exit (required)
	unsigned int mNParams;	///< Number of parameters
	bool mRun;				/// Boolean to indicate if the minimizer should continue to run.
							/// The programmer MUST check this and terminate the minimizer if false.
							///
	string mSaveDirectory; ///< Directory to which minimizer save files and exported data are saved.

	string mName;			///< Human-friendly name for the minimizer.
	string mID;				///< Unique ID for the minimizer.

	valarray<double> mChis;			///< Buffer for storing residuals
	valarray<double> mUncertainties;		///< Buffer for storing uncertainties.

	CMinimizerThread();
	virtual ~CMinimizerThread();

	static double ComputeChi2r(valarray<double> & chi, unsigned int n_params);

	virtual void ExportResults();

	virtual void GetResults(double * results, int n_params);

	string ID();
	virtual void Init(shared_ptr<CWorkerThread> worker_thread);

	string name();

	void setSaveDirectory(string save_directory);
	bool isRunning() { return mIsRunning; };

	void OpenStatisticsFile(ofstream & file);

	// Pure virtual function, each minimizer must implement this.
	virtual void run() = 0;
	virtual void stop();

	void WriteHeader(vector<string> & param_names, ofstream & outfile);

	template <class T>
	void WriteTable(vector< vector<T> > & data, ofstream & output)
	{
		// write the data to the file
		typename vector< vector<T> >::iterator row;
		typename vector<T>::iterator cell;
		int nEntries = data.size();
		// iterate over each row
		for (row = data.begin() ; row < data.end(); row++)
			WriteRow(*row, output);
	}

	template <class T>
	void WriteRow(vector<T> & data, ofstream & output)
	{
		// write the data to the file
		typename vector<T>::iterator column;
		int nEntries = data.size();
		// iterate over each row
		for(int i = 0; i < data.size() - 1; i++)
			output << data[i] << ", ";

		output << data[data.size() - 1] << endl;
	}

	static void WriteRow(double * data, unsigned int size, double chi2r, ofstream & output);
};

#endif /* CMINIMIZER_H_ */
