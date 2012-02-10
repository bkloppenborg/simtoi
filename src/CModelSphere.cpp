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
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mName = "Sphere";
	mType = CModelList::SPHERE;

	// Set the radius to some useful value, make it free.
	mParamNames.push_back("Radius");
	SetParam(mBaseParams + 1, 0.1);
	SetFree(mBaseParams + 1, true);
	SetMin(mBaseParams + 1, 0);
	SetMax(mBaseParams + 1, 100);
}

CModelSphere::~CModelSphere()
{
	// TODO Auto-generated destructor stub
}

void CModelSphere::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Rename a few variables for convenience:
	float radius = mParams[mBaseParams + 1];

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// All models need to call setup matrix to load the identity, set the model view
		// and set the correct
		SetupMatrix();

		// Load the color:
		Color();

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
	CCL_GLThread::CheckOpenGLError("CModelSphere.Render()");
}
