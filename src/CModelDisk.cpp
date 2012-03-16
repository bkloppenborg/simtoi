/*
 * CModelDisk.cpp
 *
 *  Created on: Feb 23, 2012
 *      Author: bkloppenborg
 */

#include "CModelDisk.h"

int CModelDisk::mDiskParams = 2;

CModelDisk::CModelDisk()
	: CModel(mDiskParams)
{
	InitMembers();
}

CModelDisk::CModelDisk(int additional_params)
	: CModel(mDiskParams + additional_params)
{
	InitMembers();
}

CModelDisk::~CModelDisk()
{
	delete[] mSinT;
	delete[] mCosT;
}

/// Baseclass routine, draws a cylinder with a top and bottom.
void CModelDisk::Draw()
{
	// Rename a few variables for convenience:
	const double radius = mParams[mBaseParams + 1] / 2;	// diameter / 2
	const double total_height = mParams[mBaseParams + 2];
	const double half_height = total_height/2;

	// Draw the top, sides, and bottom
	DrawDisk(radius, half_height);
	DrawSides(radius, total_height);
	DrawDisk(radius, -half_height);
}

/// Draws a flat (planar) disk
void CModelDisk::DrawDisk(double radius, double at_z)
{
    glColor4d(mParams[3], 0.0, 0.0, 1.0);
	glBegin( GL_TRIANGLE_FAN );

	if(at_z < 0)
		glNormal3d( 0.0, 0.0, -1.0 );
	else
		glNormal3d(0.0, 0.0, 1.0);

	glVertex3d( 0.0, 0.0, at_z);
	for(int j = 0; j <= mSlices; j++ )
		glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, at_z);
	glEnd();
}

/// Draws the sides of the disk (cylinder)
void CModelDisk::DrawSides(double radius, double total_height)
{
	double transparency = 0;
    double z0 = 0;
    double z1 = 0;
    double zStep = 0;
    double r0 = 0;
    double r1 = 0;
    double half_height = total_height / 2;

    // Divide the z direction into mStacks equal steps
    // Stop when we get to mScale in height.
    zStep = total_height / mStacks;

	z0 = -half_height;
	z1 = z0 + zStep;

	while(z0 < half_height)
	{
		glColor4d(mParams[3], 0.0, 0.0, Transparency(half_height, z0));
		r0 = GetRadius(half_height, z0, zStep, radius);
		r1 = GetRadius(half_height, z1, zStep, radius);

		// Draw the top half
		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ],      mSinT[ j ],      0.0 );
			glVertex3d( mCosT[ j ] * r0, mSinT[ j ] * r0, z0  );
			glVertex3d( mCosT[ j ] * r1, mSinT[ j ] * r1 ,z1  );
		}
		glEnd( );

		z0 = z1;
		z1 += zStep;
	}
}

/// Returns the transparency at the specified height.
double CModelDisk::Transparency(double total_height, double z)
{
	return 1.0;
}

/// Returns the radius as a function of height for the outer edge of the disk
/// Specify r0 to indicate the radius of the outer rim.
double CModelDisk::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	return rim_radius;
}

void CModelDisk::InitMembers()
{
	// CModel(3) because we have three additional parameters for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mStacks = 100;
	mSinT = new double[mSlices + 1];
	mCosT = new double[mSlices + 1];
	CircleTable(mSinT, mCosT, mSlices);

	mName = "Disk";
	mType = CModelList::DISK;

	mZeroThreshold = 1E-4;	// The zero point for flux

	// Set the radius and diameter to something useful:
	mParamNames.push_back("Diameter");
	SetParam(mBaseParams + 1, 3.0);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 6.0);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("Height");
	SetParam(mBaseParams + 2, 0.5);
	SetFree(mBaseParams + 2, true);
	SetMax(mBaseParams + 2, 2.0);
	SetMin(mBaseParams + 2, 0.1);
}

void CModelDisk::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Disable depth testing temporarily.
    //glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		SetupMatrix();
		Color();
		UseShader();

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		Draw();

	glPopMatrix();

    //glEnable(GL_DEPTH_TEST);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CCL_GLThread::CheckOpenGLError("CModelDisk.Render()");
}
