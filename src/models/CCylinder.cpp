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

	// Resize the texture. Single pixel is all that is required.
	mTexture.resize(1);
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
			vertices.push_back(vec3(1, 0, 0)); // texture vector
		}
	}
	// The number of vec3s that define a (complete) vertex
	unsigned int n_vec3_per_vertex = 3;

	// now generate the indices
	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	double top, bottom;
	for(unsigned int r = 0; r < r_divisions; r++)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			bottom = n_vec3_per_vertex * (r * phi_divisions + phi);
			top = n_vec3_per_vertex * ((r + 1) * phi_divisions + phi);
			elements.push_back(bottom + vertex_offset);
			elements.push_back(top + vertex_offset);
		}

		// To complete a z-row, link back to the first two vertices
		// in the row.
		bottom = n_vec3_per_vertex * (r * phi_divisions);
		top = n_vec3_per_vertex * ((r + 1) * phi_divisions);
		elements.push_back(bottom + vertex_offset);
		elements.push_back(top + vertex_offset);
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
			vertices.push_back(vec3(1, 0, 0)); // texture vector
		}
	}
	// The number of vec3s that define a vertex
	unsigned int n_vec3_per_vertex = 3;

	// now generate the indices
	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	double top, bottom;
	for(unsigned int z = 0; z < z_divisions; z++)
	{
		for(unsigned int phi = 0; phi < phi_divisions; phi++)
		{
			bottom = n_vec3_per_vertex * (z * phi_divisions + phi);
			top = n_vec3_per_vertex * ((z + 1) * phi_divisions + phi);
			elements.push_back(top + element_offset);
			elements.push_back(bottom + element_offset);
		}

		// To complete a z-row, link back to the first two vertices
		// in the row.
		bottom = n_vec3_per_vertex * (z * phi_divisions);
		top = n_vec3_per_vertex * ((z + 1) * phi_divisions);
		elements.push_back(bottom + element_offset);
		elements.push_back(top + element_offset);
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
	// As discussed in GenerateSphere_LatLon the VBO data is packed as thus:
	//
	// Positions are the first three elements
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(GLvoid *) 0);
	glEnableVertexAttribArray(posAttrib);
	// setup the normals
	GLint normAttrib = glGetAttribLocation(shader_program, "normal");
	if (normAttrib > -1)
	{
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,
				3 * sizeof(float), (GLvoid *) (3 * sizeof(float)));
	}
	// texture coordinates are packed right after the vertex coordinates.
	GLint texAttrib = glGetAttribLocation(shader_program, "tex_coords");
	glVertexAttribPointer(texAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(GLvoid *) (6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CCylinder.Init()");

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	// Load image as a texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE, mTextureID);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mTexture[0]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set sampler
	GLuint TextureSamp = glGetUniformLocation(shader_program, "TexSampler");
	glUniform1i(TextureSamp, 0); // Set "TexSampler" to user texture Unit 0

	// unbind from the texture
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	CWorkerThread::CheckOpenGLError("CCylinder.Init(), texture setup");

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

	mTexture[0].x = mParams[3];
	mTexture[0].a = 1.0;

	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Activate the shader
	GLuint shader_program = mShader->GetProgram();
	mShader->UseShader();

	// bind back to the VAO
	glBindVertexArray(mVAO);

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

	// bind to this object's texture, upload the image.
	glBindTexture(GL_TEXTURE_RECTANGLE, mTextureID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mTexture[0]);

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

	// Unbind
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelDisk.Render()");
}
