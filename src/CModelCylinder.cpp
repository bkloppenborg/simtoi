/*
 * CModelCylinder.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 *
 *  A basic cylinder.
 */

#include "CModelCylinder.h"

CModelCylinder::CModelCylinder()
	: CModel(2)
{
	// CModel(2) because we have two additional parameters for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.
	// and mParams[3] = color

	mSlices = 50;	// seems like a good number.
	mName = "Cylinder";
	mType = MDL_CYLINDER;

	// Set the radius to some useful value, make it free.
	mParamNames.push_back("Radius");
	SetParam(mBaseParams + 1, 3.0);
	SetFree(mBaseParams + 1, true);
	mParamNames.push_back("Height");
	SetParam(mBaseParams + 2, 0.5);
	SetFree(mBaseParams + 2, true);
}

CModelCylinder::~CModelCylinder()
{
	// TODO Auto-generated destructor stub
}

void CModelCylinder::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Rename a few variables for convenience:
	float cyl_radius = mParams[mBaseParams + 1];
	float cyl_height = mParams[mBaseParams + 2];

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
		// gluCylinder draws the cylinder's height with z = [0, height],
		// we want z = [-height/2, height/2] so translate down by height/2

		// Model defined drawing functions:
		GLUquadricObj * cylinder = gluNewQuadric();
		GLUquadricObj * top = gluNewQuadric();
		GLUquadricObj * bottom = gluNewQuadric();

		// First draw the top:
		glPushMatrix();
		glTranslatef(0.0, 0.0, cyl_height/2);
		gluQuadricDrawStyle(top, GLU_FILL);
		gluQuadricNormals(top, GLU_SMOOTH);
		gluQuadricOrientation(top, GLU_OUTSIDE);
		gluDisk(top, 0, cyl_radius, mSlices, mStacks);
		glPopMatrix();

		// Now the sides
		glPushMatrix();
		glTranslatef(0.0, 0.0, -cyl_height/2);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluQuadricNormals(cylinder, GLU_SMOOTH);
		gluQuadricOrientation(cylinder, GLU_OUTSIDE);
		gluCylinder(cylinder, cyl_radius, cyl_radius, cyl_height,  mSlices, mStacks);
		glPopMatrix();

		glPushMatrix();
		// Lastly the bottom:
		glTranslatef(0.0, 0.0, -cyl_height/2);
		gluQuadricDrawStyle(bottom, GLU_FILL);
		gluQuadricNormals(bottom, GLU_SMOOTH);
		gluQuadricOrientation(bottom, GLU_OUTSIDE);
		gluDisk(bottom, 0, cyl_radius, mSlices, mStacks);
		glPopMatrix();

		gluDeleteQuadric(top);
		gluDeleteQuadric(cylinder);
		gluDeleteQuadric(bottom);

	glPopMatrix();

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CCL_GLThread::CheckOpenGLError("CModelCylinder.Render()");
}
