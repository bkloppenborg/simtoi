/*
 * CModelFactory.cpp
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

#include "CModelFactory.h"
#include <stdexcept>

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
#include "models/CModelSphere.h"
#include "models/CModelDisk.h"
#include "models/CModelDisk_A.h"
#include "models/CModelDisk_B.h"
#include "models/CModelDisk_C.h"
#include "models/CModelDisk_ConcentricRings.h"

CModelFactory::CModelFactory()
{
	Register("sphere", &CModelSphere::Create);
	Register("cylinder", &CModelDisk::Create);
	Register("disk_a", &CModelDisk_A::Create);
	Register("disk_b", &CModelDisk_B::Create);
	Register("disk_c", &CModelDisk_C::Create);
	Register("disk_concentric_rings", &CModelDisk_ConcentricRings::Create);
}

CModelFactory::~CModelFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified model.
/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
shared_ptr<CModel> CModelFactory::CreateModel(string ModelID)
{
	auto it = mFactory.find(ModelID);
	if(it != mFactory.end())
		return it->second();

	throw runtime_error("The model with ID '" + ModelID + "' not registered with CModelFactory");
}

/// Returns a vector of the model names that are loaded.
vector<string> CModelFactory::GetModelList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the ModelFactory instance
CModelFactory CModelFactory::Instance()
{
	static CModelFactory instance;
	return instance;
}

/// Registers a model with the name "ModelID" and creation function "CreateFunction" with the factory.
void CModelFactory::Register(string ModelID, CreateModelFn CreateFunction)
{
	if(mFactory.find(ModelID) != mFactory.end())
		throw runtime_error("A model with ID '" + ModelID + "' is already registered with CModelFactory");

	mFactory[ModelID] = CreateFunction;
}
