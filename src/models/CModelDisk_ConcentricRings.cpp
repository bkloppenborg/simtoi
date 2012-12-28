/*
 * CModelDisk_ConcentricRings.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 */

#include "CModelDisk_ConcentricRings.h"

CModelDisk_ConcentricRings::CModelDisk_ConcentricRings()
: 	CModelDisk(5)
{
	mName = "Concentric Rings";
	mType = CModelList::DISK_CONCENTRIC_RINGS;

	// Set the members to something useful:
	mParamNames.push_back("Inner Radius");
	SetParam(mBaseParams + 1, 0.1);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 6.0);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("Outer Radius");
	SetParam(mBaseParams + 2, 0.1);
	SetFree(mBaseParams + 2, true);
	SetMax(mBaseParams + 2, 6.0);
	SetMin(mBaseParams + 2, 0.1);

	mParamNames.push_back("Alpha (r)");
	SetParam(mBaseParams + 3, 1);
	SetFree(mBaseParams + 3, true);
	SetMax(mBaseParams + 3, 10);
	SetMin(mBaseParams + 3, 0.1);

	mParamNames.push_back("Beta (r)");
	SetParam(mBaseParams + 4, 1);
	SetFree(mBaseParams + 4, true);
	SetMax(mBaseParams + 4, 10);
	SetMin(mBaseParams + 4, 0.1);

	mParamNames.push_back("N Rings (int)");
	SetParam(mBaseParams + 5, 50);
	SetFree(mBaseParams + 5, false);
	SetMax(mBaseParams + 5, 50);
	SetMin(mBaseParams + 5, 50);
}

CModelDisk_ConcentricRings::~CModelDisk_ConcentricRings()
{
	// TODO Auto-generated destructor stub
}

/// Overrides the default CModel::SetShader function.
void CModelDisk_ConcentricRings::SetShader(CGLShaderWrapperPtr shader)
{
	// Inhibits arbitrary shader assignment to this model as the radial and z-shaders are pre-determined for this model.
	// Do nothing here.
}

void CModelDisk_ConcentricRings::Render(GLuint framebuffer_object, int width, int height)
{
	// lookup and name variables:
	// Parameters from CModelDisk:
	const double total_height = mParams[mBaseParams + 2];
	const double half_height = total_height/2;

	// Parameters from CModelDisk_ConcentricRings:
	const double r_in  = mParams[mBaseParams + mDiskParams + 1];
	const double r_out = mParams[mBaseParams + mDiskParams + 2];
	const double alpha = mParams[mBaseParams + mDiskParams + 3];
	const double beta  = mParams[mBaseParams + mDiskParams + 4];
	int n_rings  = ceil(mParams[mBaseParams + mDiskParams + 5]);

	if(n_rings < 1)
		n_rings = 1;

	const double dr = (r_out - r_in) / n_rings;
	double radius = r_in;

	// Iterate over the rings, also increment the radius
	for(int i = 0; i < n_rings; i++, radius += dr)
	{
		// Draw the top, sides, and bottom
		DrawDisk(radius, half_height);
		DrawSides(radius, total_height);
		DrawDisk(radius, -half_height);
	}

}
