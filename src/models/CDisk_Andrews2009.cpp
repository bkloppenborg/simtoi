/*
 * CDisk_Andrews2009.cpp
 *
 *  Created on: Nov. 5, 2013
 *      Author: bkloppenborg
 */

#include "CDisk_Andrews2009.h"
#include "CShaderFactory.h"

CDisk_Andrews2009::CDisk_Andrews2009()
: 	CDensityDisk()
{
	mID = "disk_andrews2009";
	mName = "Flared Disk (Andrews 2009)";

	// This model ALWAYS uses the Andrews 2009 disk shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("disk_andrews2009");
}

CDisk_Andrews2009::~CDisk_Andrews2009()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CDisk_Andrews2009::Create()
{
	return shared_ptr<CModel>(new CDisk_Andrews2009());
}
