/*
 * CModelDisk_A.cpp
 *
 *  Created on: Feb 24, 2012
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

#include "CModelDisk_A.h"

CModelDisk_A::CModelDisk_A()
	:CModelDisk(1)
{
	mName = "Disk A";
	mModelID = "disk_a";

	mParamNames.push_back("Decay Factor");
	SetParam(mBaseParams + mDiskParams + 1, 0.05);
	SetFree(mBaseParams + mDiskParams + 1, true);
	SetMax(mBaseParams + mDiskParams + 1, 2.0);
	SetMin(mBaseParams + mDiskParams + 1, 0.01);
}

CModelDisk_A::~CModelDisk_A()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CModelDisk_A::Create()
{
	return shared_ptr<CModel>(new CModelDisk_A());
}

/// Returns the radius for the specified height following exponential decay
double CModelDisk_A::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	const double h_r = mParams[mBaseParams + mDiskParams + 1];
	h = fabs(h);

	if(h < dh)
		h = dh;

	return rim_radius - h_r * log(h/half_height);
}
