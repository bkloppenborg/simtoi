/*
 * CTask.cpp
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

#include "CTask.h"

CTask::CTask(CWorkerThread * WorkerThread)
{
	mWorkerThread = WorkerThread;
}

CTask::~CTask() {
	// TODO Auto-generated destructor stub
}

string CTask::GetDataDescription()
{
	return mDataDescription;
}

vector<string> CTask::GetExtensions()
{
	return mExtensions;
}

/// \brief Strips the absolute path from the filename
string CTask::StripPath(string filename)
{
	// strip off all characters before and including the last instance of a slash.
	return filename.substr(filename.find_last_of("\\/") + 1, filename.size());
}

/// \brief Strips the extension from a file.
string CTask::StripExtension(string filename, vector<string> & valid_extensions)
{
	size_t lastindex = std::string::npos;

	for(auto extension : valid_extensions)
	{
		lastindex = filename.rfind("." + extension);

		if (lastindex != std::string::npos)
			return filename.substr(0, lastindex);
	}

	return filename;
}
