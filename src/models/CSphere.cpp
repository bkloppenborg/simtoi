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

	mName = "Sphere";

	// Set the radius to some useful value, make it free.
	// NOTE: it is necessary to set max BEFORE setting min so logic test min < max works correctly.
	mParamNames.push_back("Diameter");
	SetParam(mBaseParams + 1, 1);
	SetFree(mBaseParams + 1, true);
	SetMax(mBaseParams + 1, 3.0);
	SetMin(mBaseParams + 1, 0.1);

	mNumElements = 0;

	mModelReady = false;
}

CSphere::~CSphere()
{
	glDeleteBuffers(1, &mEBO);
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

shared_ptr<CModel> CSphere::Create()
{
	return shared_ptr<CModel>(new CSphere());
}

/// Generates a sphere by dividing it into subdivisions in latitude and longitude.
/// Render with GL_TRIANGLES.
///
/// vbo_data contains the verticies and normals stored as {x,y,z,n_x,n_y,n_z}_i
void CSphere::GenerateSphere_LatLon(vector<vec3> & vbo_data, vector<unsigned int> & elements, unsigned int latitude_subdivisions, unsigned int longitude_subdivisions)
{
	// Generate a lookup table of sine/cosine values
	// Iterate theta from zero to PI (hence + 1 on the subdivisions)
	double sin_theta[latitude_subdivisions + 1];
	double cos_theta[latitude_subdivisions + 1];
	double dtheta = M_PI / latitude_subdivisions;
	for(unsigned int i = 0; i < latitude_subdivisions + 1; i++)
	{
		sin_theta[i] = sin(dtheta * i);
		cos_theta[i] = cos(dtheta * i);
	}

	double sin_phi[longitude_subdivisions];
	double cos_phi[longitude_subdivisions];
	double dphi = 2 * M_PI / longitude_subdivisions;
	for(unsigned int i = 0; i < longitude_subdivisions; i++)
	{
		sin_phi[i] = sin(dphi * i);
		cos_phi[i] = cos(dphi * i);
	}

	// Generate the vertex positions.
	double x, y, z;
	for(int i = 0; i < latitude_subdivisions + 1; i++)
	{
		for(int j = 0; j < longitude_subdivisions; j++)
		{
			x = sin_theta[i] * cos_phi[j];
			y = sin_theta[i] * sin_phi[j];
			z = cos_theta[i];
			// The first three elements are the verticies
			vbo_data.push_back(vec3(x, y, z));
		}
	}

	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	for(int lat = 0; lat < latitude_subdivisions; lat++)
	{
		for(int lon = 0; lon < longitude_subdivisions; lon++)
		{
			elements.push_back(lat * longitude_subdivisions + lon);
			elements.push_back((lat + 1) * longitude_subdivisions + lon);
		}

		// To complete a latitude row, link back to the first two vertices
		// in the row.
		elements.push_back(lat * longitude_subdivisions);
		elements.push_back((lat + 1) * longitude_subdivisions);
	}
}

void CSphere::Init()
{
	// Generate the verticies and elements
	vector<vec3> vbo_data;
	vector<unsigned int> elements;
	unsigned int latitude_subdivisions = 20;
	unsigned int longitude_subdivisions = 20;
	GenerateSphere_LatLon(vbo_data, elements, latitude_subdivisions, longitude_subdivisions);

	mNumElements = elements.size();

	// Create a new Vertex Array Object, Vertex Buffer Object, and Element Buffer
	// object to store the model's information.
	//
	// First generate the VAO, this stores all buffer information related to this object
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	// Generate and bind to the VBO. Upload the verticies.
	glGenBuffers(1, &mVBO); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vbo_data.size() * sizeof(vec3), &vbo_data[0], GL_STATIC_DRAW);
	// Generate and bind to the EBO. Upload the elements.
	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

	// Next we need to define the storage format for this object for the shader.
	// First get the shader and activate it
	GLuint shader_program = mShader->GetProgram();
	glUseProgram(shader_program);
	// Now start defining the storage for the VBO.
	// The 'vbo_data' variable stores the data as [x,y,z,n_x,n_y,n_z]_i, ...
	// where {x,y,z} are the vertices and {n_x,n_y,n_z} are the normals
	// First define the positions:
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
	// Now define the normals, if they are used
	GLint normAttrib = glGetAttribLocation(shader_program, "normal");
	if(normAttrib > -1)
	{
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
	}

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CModelSphere.Init()");

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CSphere::Render(GLuint framebuffer_object, const glm::mat4 & view)
{
	if(!mModelReady)
		Init();

	// Rename a few variables for convenience:
	double radius = float(mParams[mBaseParams + 1] / 2);

	vec2 color = vec2(mParams[3], 1.0);

	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Activate the shader
	GLuint shader_program = mShader->GetProgram();
	mShader->UseShader();
	GLint uniColorFlag = glGetUniformLocation(shader_program, "color_from_uniform");
	glUniform1i(uniColorFlag, true);

	// bind back to the VAO
	glBindVertexArray(mVAO);

	// Define the view:
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Apply scale, rotation, and translation operations:
    GLint uniModel = glGetUniformLocation(shader_program, "model");
	mat4 model = glm::scale(mat4(), vec3(radius, radius, radius));
	model = Translate() * Rotate() * model;
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

    GLint uniColor = glGetUniformLocation(shader_program, "uni_color");
    glUniform2fv(uniColor, 1, glm::value_ptr(color));

	// render
	glDrawElements(GL_TRIANGLE_STRIP, mNumElements, GL_UNSIGNED_INT, 0);

	// unbind from the Vertex Array Object, Vertex Buffer Object, and Element buffer object.
	glBindVertexArray(0);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelSphere.Render()");
}
