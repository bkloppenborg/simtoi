 /* 
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
 *
 *  Copyright (c) 2011 Brian Kloppenborg
 */

#include "CSphere.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <float.h>

#include "CShaderFactory.h"

using namespace std;

CSphere::CSphere()
	: CModel()
{
	mID = "sphere";
	mName = "Sphere";

	addParameter("T_eff", 5000, 2E3, 1E6, false, 100, "T_eff", "Effective temperature (Kelvin)", 0);
	addParameter("radius", 0.5, 0, 1, true, 0.05, "Radius", "Radius of the sphere (mas)", 4);

	mNumElements = 0;

	mFluxTexture.resize(1);	// single element texture.
	mPixelTemperatures.resize(1);

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

	// Generate the VBO. It will be populated as follows:
	// [x0, y0, z0], [xN, yN, zN],
	// because we store a unit sphere, the vertex positions are the same
	// as the normals.
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
			// normals:
			vbo_data.push_back(vec3(x, y, z));
			// define the texture coordinate location
			vbo_data.push_back(vec3(1.0, 0, 0));
		}
	}
	// The number of vec3s that define a vertex
	unsigned int n_vec3_per_vertex = 3;

	// Now assign the elements. Go in the same direction as the vertices were
	// generated above, namely in rows by latitude.
	unsigned int top = 0, bottom = 0;
	for(int lat = 0; lat < latitude_subdivisions; lat++)
	{
		for(int lon = 0; lon < longitude_subdivisions; lon++)
		{
			// compute the index of the top and bottom vertices
			bottom = lat * longitude_subdivisions + lon;
			top = (lat + 1) * longitude_subdivisions + lon;
			elements.push_back(n_vec3_per_vertex * bottom);
			elements.push_back(n_vec3_per_vertex * top);
		}

		// To complete a latitude row, link back to the first two vertices
		// in the row.
		bottom = lat * longitude_subdivisions;
		top = (lat + 1) * longitude_subdivisions;

		elements.push_back(n_vec3_per_vertex * bottom);
		elements.push_back(n_vec3_per_vertex * top);
	}
}

void CSphere::Init()
{
	// Generate the verticies and elements
	vector<vec3> vbo_data;
	vector<unsigned int> elements;
	unsigned int latitude_subdivisions = 50;
	unsigned int longitude_subdivisions = 50;
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

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Failed to create buffers");

	// Initialize the shader variables and texture following the default packing
	// scheme.
	InitShaderVariables();
	InitTexture();

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Failed bind back the default buffers");

	// Indicate the model is ready to use.
	mModelReady = true;
}


void CSphere::preRender(double & max_flux)
{
	if (!mModelReady)
		Init();

	double temperature = float(mParams["T_eff"].getValue());
	mPixelTemperatures[0] = temperature;
	TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_flux);
}

void CSphere::Render(const glm::mat4 & view, const GLfloat & max_flux)
{
	// Rename a few variables for convenience:
	double radius = float(mParams["radius"].getValue());
	mat4 scale = glm::scale(mat4(1.0f), glm::vec3(radius, radius, radius));

	NormalizeFlux(max_flux);

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
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));

	// bind to this object's texture
	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);
	// upload the image
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mFluxTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mFluxTexture[0]);

	// render
	glDrawElements(GL_TRIANGLE_STRIP, mNumElements, GL_UNSIGNED_INT, 0);

	// Unbind
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	// unbind from the Vertex Array Object, Vertex Buffer Object, and Element buffer object.
	glBindVertexArray(0);

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Rendering failed");
}

