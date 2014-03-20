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

CCylinder::CCylinder()
	: CModel(2)
{
	mName = "Cylinder";

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

	mMidplaneStart = 0;
	mMidplaneSize = 0;
	mRimStart = 0;
	mRimSize = 0;
}

CCylinder::~CCylinder()
{

}

shared_ptr<CModel> CCylinder::Create()
{
	return shared_ptr<CModel>(new CCylinder());
}

void CCylinder::GenerateMidplane(vector<vec3> & vertices, vector<unsigned int> & elements,
		unsigned int r_divisions, unsigned int phi_divisions)
{
	unsigned int element_offset = 0;
	GenerateMidplane(vertices, elements, element_offset, r_divisions, phi_divisions);
}

void CCylinder::GenerateMidplane(vector<vec3> & vertices, vector<unsigned int> & elements,
		unsigned int vertex_offset,
		unsigned int r_divisions, unsigned int phi_divisions)
{
	double sin_phi[phi_divisions];
	double cos_phi[phi_divisions];
	double dphi = 2 * M_PI / phi_divisions;
	for(unsigned int i = 0; i < phi_divisions; i++)
	{
		sin_phi[i] = sin(dphi * i);
		cos_phi[i] = cos(dphi * i);
	}

	// Generate the vertices and normals
	double dr = 1.0 / r_divisions;
	double x, y;
	// Note, we exceed the outer radius here to ensure the outer-rim is
	// drawn. This avoids some floating point issues.
	for(double r = 0; r < 1 + dr; r += dr)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			x = cos_phi[phi] * r;
			y = sin_phi[phi] * r;
			vertices.push_back(vec3(x, y, 0)); // vertex position
			vertices.push_back(vec3(0, 0, 1)); // normal vector
		}
	}

	// now generate the indices
	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	for(unsigned int r = 0; r < r_divisions; r++)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			elements.push_back(r * phi_divisions + phi + vertex_offset);
			elements.push_back((r + 1) * phi_divisions + phi + vertex_offset);
		}

		// To complete a z-row, link back to the first two vertices
		// in the row.
		elements.push_back(r * phi_divisions + vertex_offset);
		elements.push_back((r + 1) * phi_divisions + vertex_offset);
	}
}

void CCylinder::GenreateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
		unsigned int z_divisions, unsigned int phi_divisions)
{
	unsigned int element_offset = 0;
	GenreateRim(vertices, elements, element_offset, z_divisions, phi_divisions);
}

/// Draws a unit cylindrical wall in the z-direction from (z = -0.5 ... 0.5, at r = 1)
void CCylinder::GenreateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
		unsigned int element_offset,
		unsigned int z_divisions, unsigned int phi_divisions)
{
	double dz = 1.0 / z_divisions;

	double sin_phi[phi_divisions];
	double cos_phi[phi_divisions];
	double dphi = 2 * M_PI / phi_divisions;
	for(unsigned int i = 0; i < phi_divisions; i++)
	{
		sin_phi[i] = sin(dphi * i);
		cos_phi[i] = cos(dphi * i);
	}

	// generate the vertices
	double x, y;
	for(double z = -0.5; z <= 0.5; z += dz)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			x = cos_phi[phi];
			y = sin_phi[phi];
			vertices.push_back(vec3(x, y, z)); // vertex position
			vertices.push_back(vec3(x, y, 0)); // normal vector
		}
	}

	// now generate the indices
	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	for(unsigned int z = 0; z < z_divisions; z++)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			elements.push_back(z * phi_divisions + phi + element_offset);
			elements.push_back((z + 1) * phi_divisions + phi + element_offset);
		}

		// To complete a z-row, link back to the first two vertices
		// in the row.
		elements.push_back(z * phi_divisions + element_offset);
		elements.push_back((z + 1) * phi_divisions + element_offset);
	}
}

void CCylinder::Init()
{
	// Generate the verticies and elements
	vector<vec3> vbo_data;
	vector<unsigned int> elements;
	unsigned int z_divisions = 20;
	unsigned int phi_divisions = 50;
	unsigned int r_divisions = 20;

	mRimStart = 0;
	CCylinder::GenreateRim(vbo_data, elements, z_divisions, phi_divisions);
	mRimSize = elements.size();
	// Calculate the offset in vertex indexes for the GenerateMidplane function
	// to generate correct element indices.
	unsigned int vertex_offset = vbo_data.size() / 2;
	mMidplaneStart = mRimSize;
	GenerateMidplane(vbo_data, elements, vertex_offset, r_divisions, phi_divisions);
	mMidplaneSize = elements.size() - mRimSize;

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
	// The 'vbo_data' variable stores a unit cylindrical shell (i.e. r = 1,
	// h = -0.5 ... 0.5).
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	// Now define the normals, if they are used
	GLint normAttrib = glGetAttribLocation(shader_program, "normal");
	if(normAttrib > -1)
	{
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	}

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CDensityDisk.Init()");

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Indicate the model is ready to use.
	mModelReady = true;
}

void CCylinder::Render(GLuint framebuffer_object, const glm::mat4 & view)
{
	if(!mModelReady)
		Init();

	// Look up the parameters:
	const double diameter = mParams[mBaseParams + 1];
	const double radius = diameter / 2;
	const double height  = mParams[mBaseParams + 2];

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

	// Define the color
    GLint uniColor = glGetUniformLocation(shader_program, "uni_color");
    glUniform2fv(uniColor, 1, glm::value_ptr(color));

	// Define the view:
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	GLint uniTranslation = glGetUniformLocation(shader_program, "translation");
	glUniformMatrix4fv(uniTranslation, 1, GL_FALSE, glm::value_ptr(Translate()));

	GLint uniRotation = glGetUniformLocation(shader_program, "rotation");
	glUniformMatrix4fv(uniRotation, 1, GL_FALSE, glm::value_ptr(Rotate()));

	GLint uniScale = glGetUniformLocation(shader_program, "scale");
    glm::mat4 r_scale = glm::scale(mat4(), glm::vec3(radius, radius, 1.0));
    glm::mat4 h_scale = glm::scale(mat4(), glm::vec3(1.0, 1.0, height));
    glm::mat4 scale = r_scale * h_scale;
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));

	// Render
	glDisable(GL_DEPTH_TEST);

	// Draw the cylindrical wall.
	glDrawElements(GL_TRIANGLE_STRIP, mRimSize, GL_UNSIGNED_INT, 0);

	// Draw the top
	mat4 z_offset;
	z_offset = glm::translate(mat4(), vec3(0, 0, 0.5));
	scale = r_scale * h_scale * z_offset;
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));
	glDrawElements(GL_TRIANGLE_STRIP, mMidplaneSize, GL_UNSIGNED_INT, (void*) (mMidplaneStart * sizeof(float)));

	// Draw the bottom
	z_offset = glm::translate(mat4(), vec3(0, 0, -0.5));
	scale = r_scale * h_scale * z_offset;
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));
	glDrawElements(GL_TRIANGLE_STRIP, mMidplaneSize, GL_UNSIGNED_INT, (void*) (mMidplaneStart * sizeof(float)));

	glEnable(GL_DEPTH_TEST);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelDisk.Render()");
}
