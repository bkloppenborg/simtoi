/*
 * CDisk_ConcentricRings.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 */

#include "CDisk_ConcentricRings.h"
#include "CShaderFactory.h"

CDisk_ConcentricRings::CDisk_ConcentricRings()
: 	CCylinder(6 - mDiskParams)
{
	mName = "Concentric Rings";

	// WARNING: This class explicitly overrides the default parameters in CModelDisk::InitMembers
	for(int i = 0; i < 2; i++)
		mParamNames.pop_back();

	// Set the members to something useful:
	mParamNames.push_back("Inner Radius");
	SetParam(mBaseParams + 1, 0.1);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 6.0);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("Outer Radius");
	SetParam(mBaseParams + 2, 3.0);
	SetFree(mBaseParams + 2, true);
	SetMax(mBaseParams + 2, 6.0);
	SetMin(mBaseParams + 2, 0.1);

	mParamNames.push_back("Height");
	SetParam(mBaseParams + 3, 0.5);
	SetFree(mBaseParams + 3, true);
	SetMax(mBaseParams + 3, 2.0);
	SetMin(mBaseParams + 3, 0.1);

	mParamNames.push_back("Alpha (r)");
	SetParam(mBaseParams + 4, 1);
	SetFree(mBaseParams + 4, true);
	SetMax(mBaseParams + 4, 10);
	SetMin(mBaseParams + 4, 0.1);

	mParamNames.push_back("Beta (z)");
	SetParam(mBaseParams + 5, 1);
	SetFree(mBaseParams + 5, true);
	SetMax(mBaseParams + 5, 10);
	SetMin(mBaseParams + 5, 0.1);

	mParamNames.push_back("N Rings (int)");
	SetParam(mBaseParams + 6, 50);
	SetFree(mBaseParams + 6, false);
	SetMax(mBaseParams + 6, 50);
	SetMin(mBaseParams + 6, 50);

	// This model ALWAYS uses the default (pass-through) shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("default");
}

CDisk_ConcentricRings::~CDisk_ConcentricRings()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CDisk_ConcentricRings::Create()
{
	return shared_ptr<CModel>(new CDisk_ConcentricRings());
}

/// Overrides the default CModel::SetShader function.
void CDisk_ConcentricRings::SetShader(CShaderPtr shader)
{
	// This mode does not accept different shaders, do nothing here.
}

double CDisk_ConcentricRings::MidplaneTransparency(double radius)
{
	const double r_in  = mParams[mBaseParams + 1];
	const double r_out = mParams[mBaseParams + 2];
	const double alpha = mParams[mBaseParams + 4];

	return (1 - pow((radius - r_in) / (r_out - r_in), alpha));
}

void CDisk_ConcentricRings::Render(GLuint framebuffer_object, int width, int height)
{
	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Parameters from CModelDisk_ConcentricRings:
	const double r_in  = mParams[mBaseParams + 1];
	const double r_out = mParams[mBaseParams + 2];
	const double total_height = mParams[mBaseParams + 3];
	const double alpha = mParams[mBaseParams + 4];
	const double beta  = mParams[mBaseParams + 5];
	int n_rings  = ceil(mParams[mBaseParams + 6]);

	if(n_rings < 1)
		n_rings = 1;

	const double dr = (r_out - r_in) / n_rings;
	const double half_height = total_height/2;

	double min_xyz[3] = {r_in, r_in, 0};
	double max_xyz[3] = {r_out, r_out, half_height};

	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		SetupMatrix();
		Color();
		UseShader(min_xyz, max_xyz);

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		// Iterate over the rings, also increment the radius
		for(double radius = r_in; radius < r_out + dr; radius += dr)
		{
			DrawDisk(radius, radius + dr, 0);
			DrawSides(radius, total_height);
		}

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelDisk.Render()");
}

double CDisk_ConcentricRings::Transparency(double half_height, double at_z)
{
	const double beta  = mParams[mBaseParams + 5];
	return 1 - pow(abs(at_z) / half_height, beta);
}
