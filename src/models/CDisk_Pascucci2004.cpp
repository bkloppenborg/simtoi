/*
 * CDisk_Pascucci2004.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 */

#include "CDisk_Pascucci2004.h"
#include "CShaderFactory.h"

CDisk_Pascucci2004::CDisk_Pascucci2004()
: 	CDensityDisk()
{
	id = "disk_pascucci2004";
	name = "Flared Disk (Pascucci 2004)";


	// This model ALWAYS uses the default (pass-through) shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("disk_pascucci2004");
}

CDisk_Pascucci2004::~CDisk_Pascucci2004()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CDisk_Pascucci2004::Create()
{
	return shared_ptr<CModel>(new CDisk_Pascucci2004());
}
