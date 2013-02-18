/*
 * CWorkerFactory.cpp
 *
 *  Created on: Feb 10, 2013
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

#include "CTaskFactory.h"
#include <stdexcept>

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
#include "tasks/COI.h"

CTaskFactory::CTaskFactory()
{
	Register("oi", COI::Create);
}

CTaskFactory::~CTaskFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified Worker.
/// Returns a shared_ptr<CWorker> to the object if found, or throws a runtime exception.
shared_ptr<CTask> CTaskFactory::CreateWorker(string WorkerID, CWorkerPtr WorkerThread)
{
	auto it = mFactory.find(WorkerID);
	if(it != mFactory.end())
		return it->second(WorkerThread);

	throw runtime_error("The Worker with ID '" + WorkerID + "' not registered with CWorkerFactory");

	return shared_ptr<CTask>();
}

/// Returns a vector of the Worker names that are loaded.
vector<string> CTaskFactory::GetWorkerList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the WorkerFactory instance
CTaskFactory CTaskFactory::Instance()
{
	static CTaskFactory instance;
	return instance;
}

/// Registers a Worker with the name "WorkerID" and creation function "CreateFunction" with the factory.
void CTaskFactory::Register(string WorkerID, CreateTaskFn CreateFunction)
{
	if(mFactory.find(WorkerID) != mFactory.end())
		throw runtime_error("A Worker model with ID '" + WorkerID + "' is already registered with CWorkerFactory");

	mFactory[WorkerID] = CreateFunction;
}
