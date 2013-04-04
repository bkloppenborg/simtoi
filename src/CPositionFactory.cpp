/*
 * CPositionFactory.cpp
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

#include "CPositionFactory.h"
#include <stdexcept>

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
#include "positions/CPositionXY.h"
#include "positions/CPositionXYZ.h"
#include "positions/CPositionOrbit.h"

CPositionFactory::CPositionFactory()
{
	Register("xy", &CPositionXY::Create);
	Register("xyz", &CPositionXYZ::Create);
	Register("orbit_bound", &CPositionOrbit::Create);
}

CPositionFactory::~CPositionFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified Position.
/// Returns a shared_ptr<CPosition> to the object if found, or throws a runtime exception.
shared_ptr<CPosition> CPositionFactory::CreatePosition(string PositionID)
{
	auto it = mFactory.find(PositionID);
	if(it != mFactory.end())
		return it->second();

	throw runtime_error("The position model with ID '" + PositionID + "' not registered with CPositionFactory");

	return shared_ptr<CPosition>();
}

/// Returns a vector of the Position names that are loaded.
vector<string> CPositionFactory::GetPositionList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the PositionFactory instance
CPositionFactory CPositionFactory::Instance()
{
	static CPositionFactory instance;
	return instance;
}

/// Registers a Position with the name "PositionID" and creation function "CreateFunction" with the factory.
void CPositionFactory::Register(string PositionID, CreatePositionFn CreateFunction)
{
	if(mFactory.find(PositionID) != mFactory.end())
		throw runtime_error("A position model with ID '" + PositionID + "' is already registered with CPositionFactory");

	mFactory[PositionID] = CreateFunction;
}
