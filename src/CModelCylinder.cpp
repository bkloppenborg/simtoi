/*
 * CModelCylinder.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 *
 *  A basic cylinder.
 *
 *  Note: This file makes use of OpenGLUT rendering routines:
 *  	ogCircleTable
 *  	glutSolidCylinder (although with modifications)
 *
 *  , copyright notice below:
 *
 * -------------------------------------
 *
 * OpenGLUT geometry rendering methods.
 *
 * Portions copyright (C) 2004, the OpenGLUT project contributors.
 * OpenGLUT branched from freeglut in February, 2004.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CModelCylinder.h"

#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cassert>

#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795028841968
#endif

using namespace std;

CModelCylinder::CModelCylinder()
	: CModel(2)
{
	// CModel(2) because we have two additional parameters for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mStacks = 100;
	mSinT = new double[mSlices + 1];
	mCosT = new double[mSlices + 1];
	CircleTable(mSinT, mCosT, mSlices);

	mName = "Cylinder";
	mType = CModelList::CYLINDER;

	// Set the radius and diameter to something useful:
	mParamNames.push_back("Diameter");
	SetParam(mBaseParams + 1, 3.0);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 6.0);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("Height");	// that is, full height
	SetParam(mBaseParams + 2, 0.5);
	SetFree(mBaseParams + 2, true);
	SetMax(mBaseParams + 2, 3.0);
	SetMin(mBaseParams + 2, 0.1);
}

CModelCylinder::~CModelCylinder()
{
	// TODO Auto-generated destructor stub
}

/// Creates a lookup table of sine and cosine values for use in drawing
/// Taken from http://openglut.cvs.sourceforge.net/viewvc/openglut/openglut/lib/src/og_geometry.c
void CModelCylinder::CircleTable( double * sint, double * cost, const int n )
{
    int i;
    const int size = abs( n );
    double angle;

    assert( n );
    angle = 2 * PI / ( double )n;

    for( i = 0; i < size; i++ )
    {
        sint[ i ] = sin( angle * i );
        cost[ i ] = cos( angle * i );
    }

    /* Last sample is duplicate of the first */
    sint[ size ] = sint[ 0 ];
    cost[ size ] = cost[ 0 ];
}

/// Draws a cylinder with a top and bottom
/// NOTE: This function is modified from the glutSolidCylinder() function in that
/// it draws the cylinder from -height/2 ... height/2 instead of 0 ... height to
/// permit the SIMTOI shaders to operate on the object correctly.
void CModelCylinder::DrawCylinder()
{
	// Rename a few variables for convenience:
	const double radius = mParams[mBaseParams + 1] / 2;	// diameter / 2
	const double height = mParams[mBaseParams + 2];
    const double zStep = height / mStacks;
    double z0 = 0;
    double z1 = 0;

	// Draw the top
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( 0.0, 0.0, -1.0 );
	glVertex3d( 0.0, 0.0,  -height/2);
	for(int j = 0; j <= mSlices; j++ )
		glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, -height/2);
	glEnd();

	// midplane
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( 0.0, 0.0, -1.0 );
	glVertex3d( 0.0, 0.0,  0);
	for(int j = 0; j <= mSlices; j++ )
		glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, 0);
	glEnd();

	// Bottom
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( 0.0, 0.0, -1.0 );
	glVertex3d( 0.0, 0.0,  height/2);
	for(int j = 0; j <= mSlices; j++ )
		glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, height/2);
	glEnd();

	// And now for the ring/rim
	z0 = -height/2;
	z1 = z0 + zStep;

	for(int i = 1; i <= mStacks; i++ )
	{
		if( i == mStacks )
			z1 = height/2;

		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ],          mSinT[ j ],          0.0 );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, z0  );
			glVertex3d( mCosT[ j ] * radius, mSinT[ j ] * radius, z1  );
		}
		glEnd( );

		z0 = z1;
		z1 += zStep;
	}
}

void CModelCylinder::Render(GLuint framebuffer_object, int width, int height)
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

		DrawCylinder();

	glPopMatrix();

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CCL_GLThread::CheckOpenGLError("CModelCylinder.Render()");
}
