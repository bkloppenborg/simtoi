/*
 * CTaskList.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 */

#include "CTaskList.h"

#include <stdexcept>
#include <sstream>
#include <fstream>

#include "CTask.h"
#include "CTaskFactory.h"
#include "CWorkerThread.h"

using namespace std;

CTaskList::CTaskList(CWorkerThread * WorkerThread)
{
	CTaskFactory factory = CTaskFactory::Instance();

	mTasks.push_back(factory.CreateWorker("oi", WorkerThread));
	mTasks.push_back(factory.CreateWorker("photometry", WorkerThread));
}

CTaskList::~CTaskList()
{

}

void CTaskList::BootstrapNext(unsigned int maxBootstrapFailures)
{
	for(auto task: mTasks)
	{
		task->BootstrapNext(maxBootstrapFailures);
	}
}

void CTaskList::clearData()
{
	for(auto task: mTasks)
		task->clearData();
}

void CTaskList::Export(string export_folder)
{
	// Each task can optionally write information to a summary file. Be sure
	// we clear it out (in case the folder is being reused).
	ofstream summary;
	summary.open(export_folder + "summary.txt", ios::trunc | ios::in | ios::out);
	summary << "# SIMTOI export summary file." << endl;
	summary << "# Each line contains the data file, type of data, and reduced chi2 for the data file in CSV format." << endl;
	summary.close();

	for(auto task: mTasks)
		task->Export(export_folder);
}

void CTaskList::GetChi(double * chis, unsigned int size)
{
	unsigned int n_data;
	unsigned int offset = 0;
	unsigned int buffer_size = 0;
	for(auto task: mTasks)
	{
		n_data = task->GetNData();

		// Calculate how many slots the task can fill in the buffer.
		buffer_size = min(n_data, size - offset);
		task->GetChi(chis + offset, buffer_size);

		offset += n_data;
	}
}

unsigned int CTaskList::GetDataSize()
{
	// Iterate over the tasks, find out how much data they hold
	unsigned int n_data = 0;
	for(auto task: mTasks)
	{
		n_data += task->GetNData();
	}

	return n_data;
}

/// \brief Builds a list of file filters for use in open file dialogs.
///
/// Builds a vector of strings containing the valid file description and
/// data types. Each entry in the returned vector will appear as
///		"description (*.extension ... *.extension)"
vector<string> CTaskList::GetFileFilters()
{
	vector<string> output;
	stringstream temp;

	string filetype = "";
	vector<string> extensions;
	for(auto task: mTasks)
	{
		// Clear out the stringstream.
		temp.str("");
		temp.clear();

		// Get the filetype and extensions:
		temp << task->GetDataDescription() << " (";
		extensions = task->GetExtensions();
		for(string extension: extensions)
		{
			temp << "*." << extension << " ";
		}

		temp << ")";

		output.push_back(temp.str());
	}

	return output;
}

int CTaskList::GetNDataFiles()
{
	int n_data_files = -1;
	for(auto task: mTasks)
		n_data_files += task->GetNDataFiles();

	return n_data_files;
}

void CTaskList::GetUncertainties(double * uncertainties, unsigned int size)
{
	unsigned int n_data;
	unsigned int offset = 0;
	unsigned int buffer_size = 0;
	for(auto task: mTasks)
	{
		n_data = task->GetNData();

		// Calculate how many slots the task can fill in the buffer.
		buffer_size = min(n_data, size - offset);
		task->GetUncertainties(uncertainties + offset, buffer_size);

		offset += n_data;
	}
}

CDataInfo CTaskList::OpenData(string filename)
{
	// TODO: Right now we identify the task object from the file extension.
	// There is probably a better way of doing this.

	// First get the extension in lower case:
	string extension = filename.substr(filename.find_last_of(".") + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	// Now find which task handles the data type.
	for(auto task: mTasks)
	{
		for(auto datatype: task->GetExtensions())
		{
			if(extension == datatype)
			{
				return task->OpenData(filename);
			}
		}
	}

	throw runtime_error("The data type " + extension + " is not supported.");
}

void CTaskList::RemoveData(unsigned int data_index)
{
	int n_files = 0;
	for(auto task: mTasks)
	{
		n_files = task->GetNDataFiles();
		// instruct the task to remove the file if it falls within the range
		// of files that are managed by this task.
		if(data_index < n_files)
			task->RemoveData(data_index);
		else
			data_index -= n_files;
	}
}

void CTaskList::InitCL()
{
	for(auto task: mTasks)
		task->InitCL();
}

void CTaskList::InitGL()
{
	for(auto task: mTasks)
		task->InitGL();
}
