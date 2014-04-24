/*
 * CFeatureFactory.cpp
 *
 *  Created on: Apr. 23, 2014
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

#include "CFeatureFactory.h"
#include <stdexcept>

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
#include "features/CUniformSpot.h"

CFeatureFactory::CFeatureFactory()
{
	Register(CUniformSpot::GetID(), &CUniformSpot::Create);
}

CFeatureFactory::~CFeatureFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified Position.
/// Returns a shared_ptr<CPosition> to the object if found, or throws a runtime exception.
shared_ptr<CFeature> CFeatureFactory::CreateFeature(string FeatureID)
{
	auto it = mFactory.find(FeatureID);
	if(it != mFactory.end())
		return it->second();

	return shared_ptr<CFeature>();
}

/// Returns a vector of the Position names that are loaded.
vector<string> CFeatureFactory::GetFeatureList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the PositionFactory instance
CFeatureFactory CFeatureFactory::Instance()
{
	static CFeatureFactory instance;
	return instance;
}

/// Registers a Position with the name "PositionID" and creation function "CreateFunction" with the factory.
void CFeatureFactory::Register(string FeatureID, CreateFeatureFn CreateFunction)
{
	if(mFactory.find(FeatureID) != mFactory.end())
		throw runtime_error("A feature with ID '" + FeatureID + "' is already registered with CFeatureFactory");

	mFactory[FeatureID] = CreateFunction;
}
