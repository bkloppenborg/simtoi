/*
 * CModelSphere.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 */
 
 /* 
 * Copyright (c) 2011 Brian Kloppenborg
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

#include "CSphere.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <float.h>

using namespace std;

CSphere::CSphere()
	: CModel(1)
{
	// CModel(1) because we have one additional parameter for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mName = "Sphere";

	// Set the radius to some useful value, make it free.
	// NOTE: it is necessary to set max BEFORE setting min so logic test min < max works correctly.
	mParamNames.push_back("Diameter");
	SetParam(mBaseParams + 1, 1);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 3.0);
	SetMin(mBaseParams + 1, 0.1);
}

CSphere::~CSphere()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CSphere::Create()
{
	return shared_ptr<CModel>(new CSphere());
}

void CSphere::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Rename a few variables for convenience:
	double radius = float(mParams[mBaseParams + 1] / 2);

	double min_xyz[3] = {0, 0, 0};
	double max_xyz[3] = {radius, radius, radius};

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// All models need to call setup matrix to load the identity, set the model view
		// and set the correct
		SetupMatrix();

		// Load the color:
		Color();

		// Use the shader (this is valid to call even if a shader hasn't been assigned to this object).
		UseShader(min_xyz, max_xyz);

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		// Model defined drawing functions:
		GLUquadricObj * sphere = gluNewQuadric();
		gluQuadricDrawStyle( sphere, GLU_FILL);
		gluQuadricNormals( sphere, GLU_SMOOTH);
		gluQuadricOrientation( sphere, GLU_OUTSIDE);
		gluQuadricTexture( sphere, GL_TRUE);
		gluSphere(sphere, radius, mSlices, mSlices);
		gluDeleteQuadric(sphere);

	glPopMatrix();

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelSphere.Render()");
}
