/*
 * CCylinder.cpp
 *
 *  Created on: Feb 23, 2012
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

#include "CCylinder.h"

int CCylinder::mDiskParams = 2;

CCylinder::CCylinder()
	: CModel(mDiskParams)
{
	InitMembers();
}

CCylinder::CCylinder(int additional_params)
	: CModel(mDiskParams + additional_params)
{
	InitMembers();
}

CCylinder::~CCylinder()
{
	delete[] mSinT;
	delete[] mCosT;
}

shared_ptr<CModel> CCylinder::Create()
{
	return shared_ptr<CModel>(new CCylinder());
}

/// Baseclass routine, draws a cylinder with a top and bottom.
void CCylinder::Draw()
{
	// Rename a few variables for convenience:
	const double radius = mParams[mBaseParams + 1] / 2;	// diameter / 2
	const double total_height = mParams[mBaseParams + 2];
	const double half_height = total_height/2;

	// Draw the top, sides, and bottom
	DrawDisk(0, radius, half_height);
	DrawSides(radius, total_height);
	DrawDisk(0, radius, -half_height);
}

void CCylinder::DrawDisk(double radius, double at_z)
{
	DrawDisk(0, radius, at_z);
}

/// Draws a flat (planar) disk
void CCylinder::DrawDisk(double r_in, double r_out, double at_z)
{
    double color = mParams[3];
    glBegin(GL_QUAD_STRIP);

		if(at_z < 0)
			glNormal3d( 0.0, 0.0, -1.0 );
		else
			glNormal3d(0.0, 0.0, 1.0);

		for(int j = 0; j <= mSlices; j++ )
		{
			glColor4d(color, 0.0, 0.0, MidplaneTransparency(r_in));
			glVertex3d(mCosT[ j ] * r_in, mSinT[ j ] * r_in, at_z);
			glVertex3d(mCosT[ j ] * r_out, mSinT[ j ] * r_out ,at_z);
		}

	glEnd();
}

/// Draws the sides of the disk (cylinder)
void CCylinder::DrawSides(double radius, double total_height)
{
	double transparency = 0;
    double z0 = 0;
    double z1 = 0;
    double zStep = 0;
    double r0 = 0;
    double r1 = 0;
    double half_height = total_height / 2;

    double color = mParams[3];

    // Divide the z direction into mStacks equal steps
    // Stop when we get to mScale in height.
    zStep = total_height / mStacks;

	z0 = -half_height;
	z1 = z0 + zStep;

	while(z0 < half_height)
	{
		glColor4d(color, 0.0, 0.0, MidplaneTransparency(radius) * Transparency(half_height, z0));
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

/// Returns the radius as a function of height for the outer edge of the disk
/// Specify r0 to indicate the radius of the outer rim.
double CCylinder::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	return rim_radius;
}

void CCylinder::InitMembers()
{
	// CModel(3) because we have three additional parameters for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mStacks = 100;
	mSinT = new double[mSlices + 1];
	mCosT = new double[mSlices + 1];
	CircleTable(mSinT, mCosT, mSlices);

	mName = "Cylinder";

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

void CCylinder::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Rename a few variables for convenience:
	const double radius = mParams[mBaseParams + 1] / 2;	// diameter / 2
	const double total_height = mParams[mBaseParams + 2];
	const double half_height = total_height/2;

	double min_xyz[3] = {0, 0, 0};
	double max_xyz[3] = {radius, radius, half_height};

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

		Draw();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);


	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelDisk.Render()");
}
