/*
 * CDisk_Alpha1973.cpp
 *
 *  Created on: July. 22, 2015
 *      Author: j. robert parks iv
 */

#include "CDisk_Alpha1973.h"
#include "CShaderFactory.h"

CDisk_Alpha1973::CDisk_Alpha1973():CDensityDisk()
{
	mID = "disk_alpha1973";
	mName = "Flared Disk (1973)";

	// This model ALWAYS uses the Andrews 2009 disk shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("disk_alpha1973");
}

CDisk_Alpha1973::~CDisk_Alpha1973()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CDisk_Alpha1973::Create()
{
	return shared_ptr<CModel>(new CDisk_Alpha1973());
}
