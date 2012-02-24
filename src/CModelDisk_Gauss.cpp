/*
 * CModelDisk_Gauss.cpp
 *
 *  Created on: Feb 23, 2012
 *      Author: bkloppenborg
 */

#include "CModelDisk_Gauss.h"

#define ZERO_TRANS 1E-4

CModelDisk_Gauss::CModelDisk_Gauss()
	: CModel(2)
{
	// CModel(3) because we have three additional parameters for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mStacks = 100;
	mSinT = new double[mSlices + 1];
	mCosT = new double[mSlices + 1];
	CircleTable(mSinT, mCosT, mSlices);

	mName = "Disk - Gauss";
	mType = CModelList::DISK_GAUSS;

	// Set the radius and diameter to something useful:
	mParamNames.push_back("Diameter");
	SetParam(mBaseParams + 1, 3.0);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 6.0);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("Sigma");
	SetParam(mBaseParams + 2, 0.5);
	SetFree(mBaseParams + 2, true);
	SetMax(mBaseParams + 2, 1.0);
	SetMin(mBaseParams + 2, 0.01);

}

CModelDisk_Gauss::~CModelDisk_Gauss()
{
	delete[] mSinT;
	delete[] mCosT;
}

/// Draws a cylinder with a top and bottom
/// NOTE: This function is modified from the glutSolidCylinder() function in that
/// it draws the cylinder from -height/2 ... height/2 instead of 0 ... height to
/// permit the SIMTOI shaders to operate on the object correctly.
void CModelDisk_Gauss::DrawDisk()
{
	// Rename a few variables for convenience:
	const double radius = mParams[mBaseParams + 1] / 2;	// diameter / 2
	const double h_z = mParams[mBaseParams + 2];
	double transparency = 0;
    double z0 = 0;
    double z1 = 0;
    double zStep = 0;

    // Disable depth testing
    glDisable(GL_DEPTH_TEST);

	// midplane
    glColor4d(mParams[3], 0.0, 0.0, 1.0);
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( 0.0, 0.0, -1.0 );
	glVertex3d( 0.0, 0.0,  0);
	for(int j = 0; j <= mSlices; j++ )
		glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, 0);
	glEnd();

	// Divide the disk into mStacks elements of equal size:
	zStep = h_z * sqrt(-2 * log(ZERO_TRANS)) / mStacks;
	z0 = 0;
	z1 = z0 + zStep;

	transparency = 1;

	while(transparency > ZERO_TRANS)
	{
		glColor4d(mParams[3], 0.0, 0.0, transparency);

		// Draw the top half
		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ],          mSinT[ j ],          0.0 );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, z0  );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, z1  );
		}
		glEnd( );

		// Draw the bottom half
		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ],          mSinT[ j ],          0.0 );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, -z0  );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, -z1  );
		}
		glEnd( );

		z0 = z1;
		z1 += zStep;
		transparency = exp(-(z0*z0) / (2 * h_z * h_z));
	}

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
}

void CModelDisk_Gauss::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		SetupMatrix();
		Color();
		UseShader();

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		DrawDisk();

	glPopMatrix();

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CCL_GLThread::CheckOpenGLError("CModel.Render()");
}
