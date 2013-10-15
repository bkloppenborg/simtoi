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
#include "models/CSphere.h"
#include "models/CCylinder.h"
#include "models/CDisk_A.h"
#include "models/CDisk_B.h"
#include "models/CDisk_C.h"
#include "models/CDisk_ConcentricRings.h"
#include "models/CDisk_Pascucci2004.h"

/// \brief Private constructor. Call `Instance()` instead.
CModelFactory::CModelFactory()
{
	Register("sphere", &CSphere::Create);
	Register("cylinder", &CCylinder::Create);
	Register("disk_a", &CDisk_A::Create);
	Register("disk_b", &CDisk_B::Create);
	Register("disk_c", &CDisk_C::Create);
	Register("disk_concentric_rings", &CDisk_ConcentricRings::Create);
	Register("disk_pascucci2004", &CDisk_Pascucci2004::Create);
}

CModelFactory::~CModelFactory() \
{
	// TODO Auto-generated destructor stub
}

/// \brief Creates an instance of the the specified model
///
/// Attempts to create an instance of the model specified by `ModelID`. If found
/// a new `CModelPtr` is created and returned. If the model is not found
/// this function will through a `runtime_error`
CModelPtr CModelFactory::CreateModel(string ModelID)
{
	auto it = mFactory.find(ModelID);
	if(it != mFactory.end())
		return it->second();

	throw runtime_error("The model with ID '" + ModelID + "' not registered with CModelFactory");

	return CModelPtr();
}

/// \brief Get a list of the model IDs that are registered with this factory.
vector<string> CModelFactory::GetModelList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// \brief Returns a copy of the ModelFactory instance
CModelFactory CModelFactory::Instance()
{
	static CModelFactory instance;
	return instance;
}

/// \brief Registers a model with the factory
///
/// \param ModelID A unique string which identifies this minimizer.
/// \param CreateFunction A pointer to a function with returns a CModelPtr
void CModelFactory::Register(string ModelID, CreateModelFn CreateFunction)
{
	if(mFactory.find(ModelID) != mFactory.end())
		throw runtime_error("A model with ID '" + ModelID + "' is already registered with CModelFactory");

	mFactory[ModelID] = CreateFunction;
}
