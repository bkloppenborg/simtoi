/*
 * CTaskList.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 */

#include "CTaskList.h"

#include <stdexcept>
#include <sstream>

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

void CTaskList::BootstrapNext()
{
	for(auto task: mTasks)
	{
		task->BootstrapNext();
	}
}

void CTaskList::Export(string export_folder)
{
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

void CTaskList::OpenData(string filename)
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
				task->OpenData(filename);
				return;
			}
		}
	}

	throw runtime_error("The data type " + extension + " is not supported.");
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
