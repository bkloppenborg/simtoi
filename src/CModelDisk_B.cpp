/*
 * CModelDisk_A.cpp
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 */

#include "CModelDisk_B.h"

CModelDisk_B::CModelDisk_B()
	:CModelDisk(1)
{
	mName = "Disk B";
	mType = CModelList::DISK_B;

	mParamNames.push_back("Decay Factor");
	SetParam(mBaseParams + mDiskParams + 1, 0.05);
	SetFree(mBaseParams + mDiskParams + 1, true);
	SetMax(mBaseParams + mDiskParams + 1, 2.0);
	SetMin(mBaseParams + mDiskParams + 1, 0.01);
}

CModelDisk_B::~CModelDisk_B()
{
	// TODO Auto-generated destructor stub
}

/// Returns the radius for the specified height following exponential decay
double CModelDisk_B::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	const double h_r = mParams[mBaseParams + mDiskParams + 1];
	h = fabs(h);

	if(h < dh)
		h = dh;

	return rim_radius + sqrt(-2*h_r*h_r * log(h/half_height));
}
