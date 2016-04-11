 /*
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
 *
 *  Copyright (c) 2013 Brian Kloppenborg
 *
 *  A class implementing a disk model consisting of a power-law internal density
 *  profile and an exponential taper at larger radii (c.f. Andrews et al. 2009)
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
