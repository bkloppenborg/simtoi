/*
 * CModelSphere.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CModelSphere.h"

CModelSphere::CModelSphere()
{
	// Set the number of free parameters used in this object.
	n_free_parameters = 1;

	slices = 50;	// seems like a good number.

}

CModelSphere::~CModelSphere()
{
	// TODO Auto-generated destructor stub
}

void CModelSphere::GetModelParameters(float * params, int n_params)
{
	// TODO: Implement scaling for radius parameter!
	params[0] = radius;
}

int CModelSphere::GetNModelFreeParameters()
{
	return this->n_free_parameters;
}

void CModelSphere::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// Note, load custom shaders here
		// no shaders :-(

		// All models should load the modelview and identity matrix after a glPushMatrix().
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Use the shader (this is valid to call even if a shader hasn't been assigned to this object).
		UseShader();

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		this->Translate();
		this->Rotate();

		// Model defined drawing functions:
		glutSolidSphere(radius, slices, slices);
	glPopMatrix();

	// Return to the deafult framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CModelSphere::SetModelParameters(float * params, int n_params)
{
	// TODO: Check that n_params > this->n_free_params, if not throw exception
	radius = params[0];
}
