/*
 * CRocheRotator.cpp
 *
 *  Created on: Apr. 21, 2014
 *      Author: bkloppenborg
 *      Description: Roche surface for rotating/oblate spheroids.
 * 
 */

#include "CRocheRotator.h"
#include "CShaderFactory.h"

CRocheRotator::CRocheRotator() :
		CHealpixSpheroid(4)
{
	mVAO = 0;
	mVBO = 0;
	mEBO = 0;

	// Tesselation parameter for healpix, 4-6 is adequate for our uses.
	mParamNames.push_back("n_side_power");
	SetParam(mBaseParams + 1, 4);
	SetFree(mBaseParams + 1, false);
	SetMax(mBaseParams + 1, 10.0);
	SetMin(mBaseParams + 1, 1);

	// gravity at the pole (= G M / (R_pole)^2 === m/s^2)
	mParamNames.push_back("g_pole");
	SetParam(mBaseParams + 2, 10);
	SetFree(mBaseParams + 2, false);
	SetMax(mBaseParams + 2, 10000.0);
	SetMin(mBaseParams + 2, 0.01);

	// Radius at the pole (units irrelevant, normally millarcseconds)
	mParamNames.push_back("r_pole");
	SetParam(mBaseParams + 3, 1);
	SetFree(mBaseParams + 3, false);
	SetMax(mBaseParams + 3, 10.0);
	SetMin(mBaseParams + 3, 1);

	// Fraction of critical rotation rate, unitless.
	mParamNames.push_back("omega_rot");
	SetParam(mBaseParams + 4, 0.5);
	SetFree(mBaseParams + 4, false);
	SetMax(mBaseParams + 4, 10.0);
	SetMin(mBaseParams + 4, 1);
}

CRocheRotator::~CRocheRotator()
{
	glDeleteBuffers(1, &mEBO);
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

shared_ptr<CModel> CRocheRotator::Create()
{
	return shared_ptr < CModel > (new CRocheRotator());
}

/// Computes the radius of an oblate spheriod following equation 26 in
/// Cranmer et. al 1995 (or equation 12 of Aufdenberg et al. 2006)
double CRocheRotator::ComputeRadius(double polar_radius, double omega_rot, double theta)
{
	double omega_sin_theta = omega_rot * sin(theta);

	double radius = 3 * (polar_radius) / (omega_sin_theta) *
			cos((M_PI + acos(omega_sin_theta)) / 3);

	return radius;
}

/// Computes the surface gravity at an arbitrary location on the sphere
///
/// @param g_pole Gravity at the pole (m/s)
/// @param r_pole Polar radius in units matching ComputeRadius(...)
/// @param omega Fraction of critical rotation rate.
/// @param theta Angular location on the sphere
/// @param phi Angular location on the sphere
void CRocheRotator::ComputeGravity(double g_pole, double r_pole, double omega,
		double radius_at_theta, double theta, double phi,
		double & g_x, double & g_y, double & g_z, double & g_mag)
{
	// pre-compute a few quantities
	double sin_theta = sin(theta);
	double cos_theta = cos(theta);
	double sin_phi = sin(phi);
	double cos_phi = cos(phi);

	double r_ratio = radius_at_theta / r_pole;
	double c1 = 8./27 * r_ratio * omega * omega;

	// Reverse the gravitational vector
	double g_r = -1 * g_pole * (-1 / (r_ratio * r_ratio) + c1 * sin_theta * sin_theta);

	double g_theta = g_pole * c1 * sin_theta * cos_theta;

	// Compute the gravity vector magnitude
	g_mag = sqrt(g_r * g_r + g_theta * g_theta);

	// Convert g_mag into Cartesian coordinates
	g_x = g_mag * sin_theta * cos_phi;
	g_y = g_mag * sin_theta * sin_phi;
	g_z = g_mag * cos_theta;
}

void CRocheRotator::GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements)
{
	const double g_pole = mParams[mBaseParams + 2];
	const double r_pole = mParams[mBaseParams + 3];
	const double omega_rot = mParams[mBaseParams + 4];

	// Generate a unit Healpix sphere
	GenerateHealpixSphere(n_pixels, n_sides);

	// Compute the radii for the pixels and corners:
	for(unsigned int i = 0; i < pixel_radii.size(); i++)
		pixel_radii[i] = ComputeRadius(r_pole, omega_rot, pixel_theta[i]);

	for(unsigned int i = 0; i < corner_radii.size(); i++)
		corner_radii[i] = ComputeRadius(r_pole, omega_rot, corner_theta[i]);

	// Compute the gravity vector for each pixel:
	for(unsigned int i = 0; i < gravity.size(); i++)
	{
		ComputeGravity(g_pole, r_pole, omega_rot,
					pixel_radii[i], pixel_theta[i], pixel_phi[i],
					g_x[i], g_y[i], g_z[i], gravity[i]);
	}

//	// Compute the gravity darkening (based on the center of the Healpix pixel)
//	surface_gravity(&pixel_radii[0], &pixel_theta[0], &pixel_phi[0],
//			&g_x[0], &g_y[0], &g_z[0], &gravity[0], npix);
//
//	double gravity_pole = 1;
//	double g_pole_x, g_pole_y, g_pole_z;
//	triaxial_gravity(radius_pole, 0.0, 0.0, g_pole_x, g_pole_y, g_pole_z, gravity_pole);
//
//
//	surface_temperature(&pixel_temperature[0], &gravity[0], gravity_pole, npix);

//	double max_temperature = 0;
//	for(unsigned int i = 0; i < npix; i++)
//	{
//		if(pixel_temperature[i] > max_temperature)
//			max_temperature = pixel_temperature[i];
//	}
//
//	// Convert temperature into fluxes
//	TemperatureToFlux(pixel_temperature, mFluxTexture, lambda, max_temperature);
//
	GenerateVBO(n_pixels, n_sides, vbo_data);

	// Create the lookup indicies.
	GenerateHealpixVBOIndicies(n_pixels, elements);
}

void CRocheRotator::Render(GLuint framebuffer_object, const glm::mat4 & view)
{
	if (!mModelReady)
		Init();

	// See if the user change the tesselation
	unsigned int t_n_sides = floor(pow(2, mParams[mBaseParams + 1]));
	if(t_n_sides != n_sides)
	{
		n_sides = t_n_sides;
		Init();
	}

	const double g_pole = mParams[mBaseParams + 2];
	const double r_pole = mParams[mBaseParams + 3];
	const double omega_rot = mParams[mBaseParams + 4];

	mat4 scale = glm::scale(mat4(), glm::vec3(1, 1, 1));

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
	glUniformMatrix4fv(uniTranslation, 1, GL_FALSE,
			glm::value_ptr(Translate()));

	GLint uniRotation = glGetUniformLocation(shader_program, "rotation");
	glUniformMatrix4fv(uniRotation, 1, GL_FALSE, glm::value_ptr(Rotate()));

	GLint uniScale = glGetUniformLocation(shader_program, "scale");
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));

	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);

	// render
	glDrawElements(GL_TRIANGLES, mElements.size(), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	// unbind from the Vertex Array Object, Vertex Buffer Object, and Element buffer object.
	glBindVertexArray(0);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

