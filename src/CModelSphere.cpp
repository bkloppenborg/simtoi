/*
 * CModelSphere.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 */

#include "CModelSphere.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <float.h>

using namespace std;

CModelSphere::CModelSphere()
	: CModel(1)
{
	// CModel(1) because we have one additional parameter for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.

	mSlices = 50;	// seems like a good number.
	mName = "Sphere";
	mType = MDL_SPHERE;

	// Set the radius to some useful value, make it free.
	mParamNames.push_back("Radius");
	SetParam(3, 1.0);
	SetFree(3, true);
}

CModelSphere::~CModelSphere()
{
	// TODO Auto-generated destructor stub
}

void CModelSphere::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Rename a few variables for convenience:
	float radius = mParams[3];

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// All models should load the modelview and identity matrix after a glPushMatrix().
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Use the shader (this is valid to call even if a shader hasn't been assigned to this object).
		UseShader();

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
}
