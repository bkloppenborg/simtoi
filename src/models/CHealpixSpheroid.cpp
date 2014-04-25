/*
 * CRocheSpheroid.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: bkloppenborg
 */

#include "CHealpixSpheroid.h"

CHealpixSpheroid::CHealpixSpheroid(int n_params) :
	CModel(n_params)
{
	mVAO = 0;
	mVBO = 0;
	mEBO = 0;

	n_sides = 4;
	n_pixels = 0;
}

CHealpixSpheroid::~CHealpixSpheroid()
{
	if(mEBO) glDeleteBuffers(1, &mEBO);
	if(mVBO) glDeleteBuffers(1, &mVBO);
	if(mVAO) glDeleteVertexArrays(1, &mVAO);
}

/// Finds pixels on the surface of the sphere using sphere-sphere intersection
/// testing.
void CHealpixSpheroid::FindPixels(double radius, double theta, double phi,
			double d_radius, double d_theta, double d_phi,
			vector<unsigned int> &pixels_ids)
{
	// Look up the (x,y,z) position of the target (r, theta, phi) center.
	long target_pixel = 0;
	ang2pix_nest(n_sides, theta, phi, &target_pixel);
	double pixel_radius = pixel_radii[target_pixel];

	vec3 temp = pixel_xyz[target_pixel];

	double x = pixel_radius * cos(phi) * sin(theta);
	double y = pixel_radius * sin(phi) * sin(theta);
	double z = pixel_radius * cos(theta);

	vec3 target_xyz = vec3(x,y,z);
	//target_xyz = normalize(target_xyz);

//	cout << "us :   :" << target_xyz.x << " " << target_xyz.y << " " << target_xyz.z << endl;

	// Compute the maximum allowable distance
	double polar_radius = mParams[mBaseParams + 3];
	double target_radius = polar_radius * sqrt(d_theta * d_theta + d_phi * d_phi);

	vec3 t_pix_xyz;

	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
	{
		t_pix_xyz = pixel_xyz[i];
		t_pix_xyz *= pixel_radii[i];

		double distance = length(target_xyz - t_pix_xyz);

		if(distance <= target_radius)
			pixels_ids.push_back(i);
	}

}


/// Creates a Healpix sphere by computing the pixel and coordinate vector
/// locations and (phi, theta) values.
void CHealpixSpheroid::GenerateHealpixSphere(unsigned int n_pixels, unsigned int n_sides)
{
	// Resize the input vectors to match the image.
	mFluxTexture.resize(n_pixels);
	pixel_xyz.resize(n_pixels);
	pixel_phi.resize(n_pixels);
	pixel_theta.resize(n_pixels);
	pixel_radii.resize(n_pixels);
//	pixel_temperatures.resize(n_pixels);

	corner_xyz.resize(4 * n_pixels);	// four corners per Healpix pixel
	corner_theta.resize(4 * n_pixels);	// four corners per Healpix pixel
	corner_phi.resize(4 * n_pixels);	// four corners per Healpix pixel
	corner_radii.resize(4 * n_pixels);	// four corners per Healpix pixel

	// Temporary double vectors to interface with Healpix's routines:
	vector<double> t_pixel_xyz(3);
	vector<double> t_corner_xyz(12);

	// Iterate over each pixel in the Healpix image
	for(unsigned int i = 0; i < n_pixels; i++)
	{
		// Init the flux texture to something useful.
		mFluxTexture[i].r = float(i) / n_pixels;
		mFluxTexture[i].a = 1.0;

		// Compute the vertex locations for the center and (four) corners
		// of the pixels.
		pix2vec_nest(n_sides, i, &t_pixel_xyz[0], &t_corner_xyz[0]);

		// Copy the pixel location from the temporary buffer into the storage buffer.
		pixel_xyz[i] = vec3(t_pixel_xyz[0], t_pixel_xyz[1], t_pixel_xyz[2]);

		// Compute the (theta, phi) values for the center of each pixel
		pix2ang_nest(n_sides, i, &pixel_theta[i], &pixel_phi[i]);

		// Compute the (theta, phi) values for each of the (four) corners
		for(unsigned int j = 0; j < 4; j++)
		{
			vec2ang(&t_corner_xyz[3*j], &corner_theta[4*i + j], &corner_phi[4*i + j]);
			// Copy the corner location into the storage buffer
			corner_xyz[4*i + j] = vec3(t_corner_xyz[3*j + 0], t_corner_xyz[3*j + 1], t_corner_xyz[3*j + 2]);
		}
	}
}

/// Generates the element buffer indicies for a Healpix sphere
void	CHealpixSpheroid::GenerateHealpixVBOIndicies(unsigned int n_pixels, vector<unsigned int> & elements)
{
	// Clear out the element buffer:
	elements.clear();

	// Each Healpix pixel is composed of four verticies which are represented
	// as vec3. Thus, the start of each Healpix vertex definition follows a
	// stride of 4 * 3 = 12 floating point numbers.
	// Because we use pix2vec_nest Healpix representation, each pixel is best
	// defined as an OpenGL "quad" composed of two triangles. The pix2vec_nest
	// function provides the vertices in a counter-clockwise fashion which we
	// identify with indices (0,1,2,3). Therefore the two triangles have
	// vertex indices (0,1,3) and (3,1,2). Each vertex is a vec3, thus these
	// vertex indices are multiplied by 3 to yield (0,3,9) and (9,3,6).

	for (unsigned int i = 0; i < n_pixels; i++)
	{
		elements.push_back(12 * i + 0);
		elements.push_back(12 * i + 3);
		elements.push_back(12 * i + 9);
		elements.push_back(12 * i + 9);
		elements.push_back(12 * i + 3);
		elements.push_back(12 * i + 6);
	}
}

/// Creates the VBO from the corners, radii, and gravity buffers.
void CHealpixSpheroid::GenerateVBO(unsigned int n_pixels, unsigned int n_side, vector<vec3> & vbo_data)
{
	// Clear out the VBO buffer
	vbo_data.clear();

	// Iterate over each Healpix pixel
	for (unsigned int i = 0; i < n_pixels; i++)
	{
		// There are four corners per pixel to define
		for (unsigned int j = 0; j < 4; j++)
		{
			// Convert the unit-radius vertices to Roche surface radii
			// by scaling and append to the vector.
			vbo_data.push_back(corner_xyz[4*i + j] * float(corner_radii[i * 4 + j]));

			// set the surface normals, remember to normalize!
			vbo_data.push_back( glm::normalize( vec3(g_x[i], g_y[i], g_z[i]) ) );

			// set the texture coordinates
			vbo_data.push_back(vec3(i % (12 * n_side), i / (12 * n_side), 0));
		}
	}
}


void CHealpixSpheroid::Init()
{
	n_pixels = nside2npix(n_sides);

	gravity.resize(n_pixels);
	g_x.resize(n_pixels);
	g_y.resize(n_pixels);
	g_z.resize(n_pixels);
	mPixelTemperatures.resize(n_pixels);

	// Generate the verticies and elements
	GenerateModel(mVBOData, mElements);
	// Create a new Vertex Array Object, Vertex Buffer Object, and Element Buffer
	// object to store the model's information.
	//
	// First generate the VAO, this stores all buffer information related to this object
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO); // Generate 1 buffer
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	// Generate and bind to the VBO. Upload the verticies.
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVBOData.size() * sizeof(vec3), &mVBOData[0],
			GL_DYNAMIC_DRAW);

	// Generate and bind to the EBO. Upload the elements.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mElements.size() * sizeof(unsigned int), &mElements[0],
			GL_DYNAMIC_DRAW);

	InitShaderVariables();

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	// Load image as a texture
	glGenTextures(1, &mFluxTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, 12 * n_sides, n_sides, 0, GL_RGBA,
			GL_FLOAT, &mFluxTexture[0]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set sampler
	GLuint shader_program = mShader->GetProgram();
	GLuint TextureSamp = glGetUniformLocation(shader_program, "TexSampler");
	glUniform1i(TextureSamp, 0); // Set "TexSampler" to user texture Unit 0

	// Return to the default texture.
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CRocheSpheroid.Init()");

	// Indicate the model is ready to use.
	mModelReady = true;
}
