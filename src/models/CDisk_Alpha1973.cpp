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
 *  Copyright (c) 2015 J. Robert Parks IV
 *
 *  A class implementing a disk model consisting of a power-law internal density
 *  profile and an exponential taper at larger radii.
 *  Based upon the work of Shakura, N. I.; Sunyaev, R. A. 1973
 *  http://adsabs.harvard.edu/abs/1973A%26A....24..337S
 */

#include "CDisk_Alpha1973.h"
#include "CShaderFactory.h"

CDisk_Alpha1973::CDisk_Alpha1973():CDensityDisk()
{
	mID = "disk_alpha1973";
	mName = "Flared Disk (Shakura 1973)";

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
