/*
 * CPositionFactory.cpp
 *
 *  Created on: Feb 10, 2013
 *      Author: bkloppenborg
 */

#include "CPositionFactory.h"
#include <stdexcept>

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
#include "positions/CPositionXY.h"
#include "positions/CPositionXYZ.h"
#include "positions/CPositionOrbit.h"

CPositionFactory::CPositionFactory()
{
	Register(CPositionXY::GetID(), &CPositionXY::Create);
	Register(CPositionXYZ::GetID(), &CPositionXYZ::Create);
	Register(CPositionOrbit::GetID(), &CPositionOrbit::Create);
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
