/*
 * CModelSphere.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CModelSphere.h"

CModelSphere::CModelSphere()
{
	// TODO Auto-generated constructor stub

}

CModelSphere::~CModelSphere()
{
	// TODO Auto-generated destructor stub
}

void CModelSphere::Render(GLuint framebuffer_object, int width, int height)
{
	double radius = 3.0;
	int slices = 50;
	double x = 5.0;

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// Note, load custom shaders here
		// no shaders :-(

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Call base-class rotation and translation functions.
		this->Translate();
		this->Rotate();

		// Model defined drawing functions:
		glutSolidSphere(radius, slices, slices);
	glPopMatrix();

	// Return to the deafult framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
