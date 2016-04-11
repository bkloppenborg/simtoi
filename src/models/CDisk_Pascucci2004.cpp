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
 *  Copyright (c) 2012 Brian Kloppenborg
 *
 *  A class implementing a disk model consisting of a series of partially
 *  opaque concentric rings. Each ring has a scale height transparency which
 *  decays from the midplane transparency to 1 (completely transparent)
 *  following a Gaussian distribution.
 *
 *  The model has the following parameters:
 *  	r_in : the inner radius (>= 0)
 *  	r_out: the outer radius (> 0)
 *  	midplane_trans : the transparency of the midplane (0...1), normally 0 (completely opaque)
 *  	beta : the Gaussian decay coefficient in the z-direction (> 0)
 *  	n_rings: the number of concentric rings between r_in and r_out (> 0)
 *
 *  NOTE: If beta is set outside of their specified bounds, the transparency
 *  in the corresponding direction will be set to zero (i.e. 100% opaque).
 *
 *  WARNING: This class explicitly overrides the default parameters in CModelDisk::InitMembers
 */

#include "CDisk_Pascucci2004.h"
#include "CShaderFactory.h"

CDisk_Pascucci2004::CDisk_Pascucci2004()
: 	CDensityDisk()
{
	mID = "disk_pascucci2004";
	mName = "Flared Disk (Pascucci 2004)";


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
