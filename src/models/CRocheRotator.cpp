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
 *  Copyright (c) 2014 Brian Kloppenborg
 *
 *  Description:
 *  The following class implements the equations for the Roche surface of a
 *  rapidly rotating star under the assumption that the mass is centrally
 *  concentrated and uniformly rotating. This code follows the equations
 *  originally specified in
 *  	Cranmer, S. R., & Owocki, S. P. (1995)
 *  	The Astrophysical Journal, 440, 308. doi:10.1086/175272
 *  and
 *  	Aufdenberg, J. P., Merand, A., Foresto, V. C. et al. (2006)
 *  	The Astrophysical Journal, 645(1), 664–675. doi:10.1086/504149
 *  to implement the gravity and temperature profiles respectively.
 */

#include "CRocheRotator.h"
#include "CShaderFactory.h"
#include "CFeature.h"

CRocheRotator::CRocheRotator() :
		CHealpixSpheroid()
{
	mID = "roche_rotator";
	mName = "Roche Rotator";

	// Tesselation parameter for healpix, 4-6 is adequate for our uses.
	addParameter("g_pole", 1, 0.01, 10000, false, 10, "Polar Gravity", "Gravity at the pole (units: m/s^2)", 2);
	addParameter("omega_rot", 0.5, 0, 1, false, 0.1, "omega", "Fraction of critical rotational velocity [range: 0...1]", 2);
	addParameter("T_eff_pole", 5000, 2E3, 1E6, false, 100, "T_pole", "Effective Polar temperature (kelvin)", 0);
	addParameter("von_zeipel_beta", 0.5, 0.01, 1.0, false, 0.1, "Beta", "Von Zeipel gravity darkening parameter (unitless)", 2);
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

	if(omega_rot < 1E-5)
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
	double g_r = -1 * g_pole * (-1.0 / (r_ratio * r_ratio) + c1 * sin_theta * sin_theta);

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
	const double g_pole = mParams["g_pole"].getValue();
	const double r_pole = mParams["r_pole"].getValue();
	const double omega_rot = mParams["omega_rot"].getValue();
	const double T_eff_pole = mParams["T_eff_pole"].getValue();
	const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();
	const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());

	// Generate a unit Healpix sphere
	GenerateHealpixSphere(n_pixels, n_sides);

	// recomputing the sphereoid is very expensive. Make use of the dirty flags
	// to only compute that which is absolutely necessary.
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
	TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_temperature);

	GenerateVBO(n_pixels, n_sides, vbo_data);

	// Create the lookup indicies.
	GenerateHealpixVBOIndicies(n_pixels, elements);
}

void CRocheRotator::preRender(double & max_flux)
{
	if (!mModelReady)
		Init();

	// See if the user change the tesselation
	const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());
	if(mParams["n_side_power"].isDirty())
	{
		Init();
	}

	const double g_pole = mParams["g_pole"].getValue();
	const double r_pole = mParams["r_pole"].getValue();
	const double omega_rot = mParams["omega_rot"].getValue();
	const double T_eff_pole = mParams["T_eff_pole"].getValue();
	const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();

	if(mParams["r_pole"].isDirty() || mParams["omega_rot"].isDirty())
		ComputeRadii(r_pole, omega_rot);

	if(mParams["g_pole"].isDirty() || mParams["r_pole"].isDirty() || mParams["omega_rot"].isDirty())
		ComputeGravity(g_pole, r_pole, omega_rot);

	bool feature_dirty = false;
	for(auto feature: mFeatures)
		feature_dirty |= feature->isDirty();

	if(feature_dirty || mParams["T_eff_pole"].isDirty() || mParams["g_pole"].isDirty() || mParams["von_zeipel_beta"].isDirty())
		VonZeipelTemperatures(T_eff_pole, g_pole, von_zeipel_beta);

	for(auto feature: mFeatures)
		feature->apply(this);

	TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_flux);

	GenerateVBO(n_pixels, n_sides, mVBOData);
}

void CRocheRotator::Render(const glm::mat4 & view, const GLfloat & max_flux)
{
	const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());

	NormalizeFlux(max_flux);

	mat4 scale = glm::scale(mat4(1.0f), glm::vec3(1, 1, 1));

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

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Rendering failed");
}

void CRocheRotator::VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta)
{
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
		mPixelTemperatures[i] = T_eff_pole * pow(gravity[i] / g_pole, beta);
}
