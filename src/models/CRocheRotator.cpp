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
#include "CFeature.h"

CRocheRotator::CRocheRotator() :
		CHealpixSpheroid(7)
{
	mName = "Roche Rotator";

	// Tesselation parameter for healpix, 4-6 is adequate for our uses.
	mParamNames.push_back("n_side_power");
	SetParam(mBaseParams + 1, 4);
	SetFree(mBaseParams + 1, false);
	SetMax(mBaseParams + 1, 10.0);
	SetMin(mBaseParams + 1, 1);

	// gravity at the pole (= G M / (R_pole)^2 === m/s^2)
	mParamNames.push_back("g_pole");
	SetParam(mBaseParams + 2, 1);
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
	SetMax(mBaseParams + 4, 1.0);
	SetMin(mBaseParams + 4, 0.01);

	// The effective temperature at the pole
	mParamNames.push_back("T_eff_pole");
	SetParam(mBaseParams + 5, 5000);
	SetFree(mBaseParams + 5, false);
	SetMax(mBaseParams + 5, 1E6);
	SetMin(mBaseParams + 5, 2E3);

	// The von Zeipel gravity darkening coefficient.
	mParamNames.push_back("von_zeipel_beta");
	SetParam(mBaseParams + 6, 0.5);
	SetFree(mBaseParams + 6, false);
	SetMax(mBaseParams + 6, 1.0);
	SetMin(mBaseParams + 6, 0.01);

	// Rotational period (units 1/day)
	mParamNames.push_back("Omega_dot");
	SetParam(mBaseParams + 7, 0.5);
	SetFree(mBaseParams + 7, false);
	SetMax(mBaseParams + 7, 1.0);
	SetMin(mBaseParams + 7, 0.01);

	// TODO: Remove this variable
	lambda = 1.4e-6; // m, wavelength of observation, used to convert temperatures to fluxes

	mTime = 0;
}

CRocheRotator::~CRocheRotator()
{

}

shared_ptr<CModel> CRocheRotator::Create()
{
	return shared_ptr < CModel > (new CRocheRotator());
}

/// Computes the radius of an oblate spheriod following equation 26 in
/// Cranmer et. al 1995 (or equation 12 of Aufdenberg et al. 2006)
double CRocheRotator::ComputeRadius(double polar_radius, double omega_rot, double theta)
{
	double sin_theta = sin(theta);

	if(abs(sin_theta) < 1E-3)	// ~ 0.1 degrees from the pole
		return polar_radius;

	double omega_sin_theta = omega_rot * sin_theta;

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
	g_mag = std::sqrt(g_r * g_r + g_theta * g_theta);

	// Convert g_mag into Cartesian coordinates
	g_x = g_mag * sin_theta * cos_phi;
	g_y = g_mag * sin_theta * sin_phi;
	g_z = g_mag * cos_theta;
}

void CRocheRotator::ComputeRadii(double r_pole, double omega_rot)
{
	// Compute the radii for the pixels and corners:
	for(unsigned int i = 0; i < pixel_radii.size(); i++)
		pixel_radii[i] = ComputeRadius(r_pole, omega_rot, pixel_theta[i]);

	for(unsigned int i = 0; i < corner_radii.size(); i++)
		corner_radii[i] = ComputeRadius(r_pole, omega_rot, corner_theta[i]);
}

void CRocheRotator::ComputeGravity(double g_pole, double r_pole, double omega_rot)
{
	// Compute the gravity vector for each pixel:
	for(unsigned int i = 0; i < gravity.size(); i++)
	{
		ComputeGravity(g_pole, r_pole, omega_rot,
					pixel_radii[i], pixel_theta[i], pixel_phi[i],
					g_x[i], g_y[i], g_z[i], gravity[i]);
	}
}

void CRocheRotator::GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements)
{
	const double g_pole = mParams[mBaseParams + 2];
	const double r_pole = mParams[mBaseParams + 3];
	const double omega_rot = mParams[mBaseParams + 4];
	const double T_eff_pole = mParams[mBaseParams + 5];
	const double von_zeipel_beta = mParams[mBaseParams + 6];

	// Generate a unit Healpix sphere
	GenerateHealpixSphere(n_pixels, n_sides);

	ComputeRadii(r_pole, omega_rot);
	ComputeGravity(g_pole, r_pole, omega_rot);
	VonZeipelTemperatures(T_eff_pole, g_pole, von_zeipel_beta);

	for(auto feature: mFeatures)
		feature->apply(this);

	// Find the maximum temperature
	double max_temperature = 0;
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
	{
		if(mPixelTemperatures[i] > max_temperature)
			max_temperature = mPixelTemperatures[i];
	}
	// Convert temperatures to fluxes.
	TemperatureToFlux(mPixelTemperatures, mFluxTexture, lambda, max_temperature);

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
	const double T_eff_pole = mParams[mBaseParams + 5];
	const double von_zeipel_beta = mParams[mBaseParams + 6];

	ComputeRadii(r_pole, omega_rot);
	ComputeGravity(g_pole, r_pole, omega_rot);
	VonZeipelTemperatures(T_eff_pole, g_pole, von_zeipel_beta);

	for(auto feature: mFeatures)
		feature->apply(this);

	double max_temperature = 0;
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
	{
		if(mPixelTemperatures[i] > max_temperature)
			max_temperature = mPixelTemperatures[i];
	}

	TemperatureToFlux(mPixelTemperatures, mFluxTexture, lambda, max_temperature);

	GenerateVBO(n_pixels, n_sides, mVBOData);

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

	// Bind to the texture, upload it.
	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, 12 * n_sides, n_sides, 0, GL_RGBA,
			GL_FLOAT, &mFluxTexture[0]);

	// Upload the VBO data:
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVBOData.size() * sizeof(vec3), &mVBOData[0],
			GL_DYNAMIC_DRAW);

	// render
	glDrawElements(GL_TRIANGLES, mElements.size(), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	// unbind from the Vertex Array Object, Vertex Buffer Object, and Element buffer object.
	glBindVertexArray(0);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CWorkerThread::CheckOpenGLError("CRocheRotator.Render()");
}

void CRocheRotator::VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta)
{
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
		mPixelTemperatures[i] = T_eff_pole * pow(gravity[i] / g_pole, beta);
}

void CRocheRotator::SetTime(double time)
{
	// Call the base class method (updates orbital parameters, etc.)
	CModel::SetTime(time);

	// Get the rotation rate:
	const double Omega_dot = mParams[mBaseParams + 7];

	// Compute the change in rotational angle
	double dt = time - mTime;
	mParams[2] = mParams[2] + dt * Omega_dot;

	// Update the current time.
	mTime = time;
}
