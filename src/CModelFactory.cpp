/*
 * CModelFactory.cpp
 *
 *  Created on: Feb 10, 2013
 *      Author: bkloppenborg
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
	Register("Sphere", &CModelSphere::Create);
	Register("Cylinder", &CModelDisk::Create);
	Register("Disk A", &CModelDisk_A::Create);
	Register("Disk B", &CModelDisk_B::Create);
	Register("Disk C", &CModelDisk_C::Create);
	Register("Disk - Concentric Rings", &CModelDisk_ConcentricRings::Create);
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

	throw runtime_error("Model " + ModelID + " not registered.");
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
		throw runtime_error("Model " + ModelID + " is already registered!");

	mFactory[ModelID] = CreateFunction;
}
