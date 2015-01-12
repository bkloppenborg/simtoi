/*
 * CRocheBinary.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: fbaron
 *      Description: basic Roche lobe with co-aligned self-rotation and orbital rotation
 *                   and non-synchronous rotation periods
 *      Note:   most of the code is not optimized at the moment...
 */

#include "CShaderFactory.h"
#include "CFeature.h"
#include "CRocheLobe.h"


CRocheLobe::CRocheLobe() :
	AU(1.496e11), rsun(6.955e8), G(6.67428e-11), parsec(3.08567758e16),
	CHealpixSpheroid()
{
	id = "roche_lobe";
	name = "Roche Lobe";

	addParameter("g_pole", 1, 0.01, 10000, false, 10, "Polar Gravity", "Gravity at the pole (units: m/s^2)");
	addParameter("T_eff_pole", 5000, 2E3, 1E6, false, 100, "T_pole", "Effective Polar temperature (kelvin)");
	addParameter("von_zeipel_beta", 0.25, 0.0, 1.0, false, 0.1, "Beta", "Von Zeipel gravity darkening parameter (unitless)");
	addParameter("separation", 4.0 , 0.1, 100.0, false, 0.01, "Separation", "Separation between components (mas)");
	addParameter("mass_ratio", 3.0 , 0.001, 100.0, false, 0.01, "Mass ratio", "Mass ratio M2/M1) (unitless)");
	addParameter("asynchronism_ratio", 1.0 , 0.01, 2.0, false, 0.01, "Async ratio", "Asynchronous ratio rotation/revolution (unitless)");
	//	omega_rot = 2.0 * PI / (orbital_period * 3600. * 24.); // in Hz
}

CRocheLobe::~CRocheLobe()
{

}

shared_ptr<CModel> CRocheLobe::Create()
{
	return shared_ptr < CModel > (new CRocheLobe());
}

void CRocheLobe::ComputeRadii(const double r_pole, const double separation, const double mass_ratio, const double asynchronism_ratio)
{


	// Compute the radii for the pixels and corners:
	for(unsigned int i = 0; i < pixel_radii.size(); i++)
	  pixel_radii[i] = ComputeRadius(r_pole, separation, mass_ratio, asynchronism_ratio, pixel_theta[i], pixel_phi[i]);

	for(unsigned int i = 0; i < corner_radii.size(); i++)
	  corner_radii[i] = ComputeRadius(r_pole, separation, mass_ratio, asynchronism_ratio, corner_theta[i], corner_phi[i]);
}

void CRocheLobe::ComputeGravity(const double g_pole, const double r_pole, const double separation, const double mass_ratio, const double asynchronism_ratio)
{
	double gnorm, tempx, tempy, tempz;
	ComputeGravity(1., r_pole, separation, mass_ratio, asynchronism_ratio, r_pole, 0., 0., tempx, tempy, tempz, gnorm);

	// ensure gravity is positive (this model can have non-physical solutions).
	assert(gnorm > 0);

    // Compute the gravity vector for each pixel:
	for(unsigned int i = 0; i < gravity.size(); i++)
	{
		ComputeGravity(g_pole / gnorm, r_pole, separation, mass_ratio, asynchronism_ratio, pixel_radii[i], pixel_theta[i], pixel_phi[i], g_x[i], g_y[i], g_z[i], gravity[i]);
	}
}

void CRocheLobe::VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta)
{
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
		mPixelTemperatures[i] = T_eff_pole * pow(gravity[i] / g_pole, beta);
}

/// Computes the tangential components and magnitude of gravity at the
/// specified (r, theta, phi) coordinates.
void CRocheLobe::ComputeGravity(const double g_pole, const double r_pole, const double separation, const double mass_ratio, const double asynchronism_ratio, const double radius, const double theta, const double phi, double & g_x, double & g_y, double & g_z, double & g_mag)
{

  double radius1 = radius / separation;  // dimensionless
  double l = cos(phi) * sin(theta);
  double mu = sin(phi) * sin(theta);
  double x = radius1 * l;
  double y = radius1 * mu;
  double z = radius1 * cos(theta);
  double xc =  mass_ratio / (1. + mass_ratio);
  double radius2 = std::sqrt( (x - 1.0) * (x - 1.0) + y * y + z * z);

  double radius1_pow3 = radius1 * radius1 * radius1;
  double radius2_pow3 = radius2 * radius2 * radius2;

  // gx, gy, gz are the cartesian coordinates of the gravity vector
  // we need them in cartesian form to compute the vertices for limb-darkening

   g_x =  x / radius1_pow3 + mass_ratio * (x - 1.0) / radius2_pow3 + (mass_ratio + 1.0) * (x-xc) + (mass_ratio + 1.0) * (asynchronism_ratio * asynchronism_ratio -1.) * x;
   g_y =  y / radius1_pow3 + mass_ratio * y         / radius2_pow3 + (mass_ratio + 1.0) *  y     + (mass_ratio + 1.0) * (asynchronism_ratio * asynchronism_ratio -1.) * y;
   g_z =  z / radius1_pow3 + mass_ratio * z         / radius2_pow3;

   g_mag = g_pole * std::sqrt(g_x * g_x + g_y * g_y + g_z * g_z);
}


void CRocheLobe::ComputePotential(double & pot, double & dpot, const double radius, const double theta, const double phi, const double separation, const double mass_ratio, const double asynchronism_ratio)
{
	// This is only valid for circular, aligned, asynchronous rotation, and will be replaced by Sepinsky 2007
	// theta in radians: co-latitude -- vector
	// phi in radians: phi phi=0 points toward companion -- vector

	const double radius1 = radius / separation;  // dimensionless
	const double l = cos(phi) * sin(theta);
	const double mu = sin(phi) * sin(theta);
	const double x = radius1 * l;
	const double y = radius1 * mu;
	const double z = radius1 * cos(theta);
	const double xc =  mass_ratio / (1. + mass_ratio);
	const double radius2 = std::sqrt( (x - 1.0) * (x - 1.0) + y * y + z * z);

	pot = - 1.0 / radius1 - mass_ratio / radius2
          - 0.5 * (mass_ratio + 1.0) * ( (x - xc) * (x - xc) + y * y)
          - 0.5 * (mass_ratio + 1.0) * (asynchronism_ratio * asynchronism_ratio -1.) * (x * x + y * y );

	dpot =  1.0 / (radius1 * radius1) + mass_ratio / (radius2 * radius2 * radius2) * (radius1 - l)
	        - (mass_ratio + 1.0) * (radius1 * (l * l + mu * mu)   - l * xc)
            - (mass_ratio + 1.0) * (asynchronism_ratio * asynchronism_ratio -1.) * radius1 * ( l * l + mu * mu);
}


double CRocheLobe::ComputeRadius(const double r_pole, const double separation, const double mass_ratio, const double asynchronous_ratio, const double theta, const double phi)
{
	// in this function we compute the roche radius based on masses/ distance / orbital_period, for each (theta, phi)
	const double epsilon = 1;
	register int i;
	double pot_surface, pot, dpot;
	double newton_step;

	ComputePotential(pot_surface, dpot, r_pole, 0.0, 0.0, separation, mass_ratio, asynchronous_ratio ); // potential at the equator

	double radius = 1.22 * r_pole; // initial guess for the radius, TBD: improve !

	bool converged = false;
	for(int i=0; i<8;i++)//while(converged == FALSE)
	{
		ComputePotential(pot, dpot, radius, theta, phi, separation, mass_ratio, asynchronous_ratio );
		newton_step = (pot - pot_surface) / dpot; // newton step
		radius = radius* (1. - newton_step);

		if (fabs(newton_step) < epsilon)
			converged = true;
	}
	return radius;
}

void CRocheLobe::Render(const glm::mat4 & view)
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
	const double T_eff_pole = mParams["T_eff_pole"].getValue();
	const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();
	const double separation = mParams["separation"].getValue();
	const double mass_ratio = mParams["mass_ratio"].getValue();
	const double asynchronism_ratio = mParams["asynchronism_ratio"].getValue();

	//	if(mParams["r_pole"].isDirty() || mParams["omega_rot"].isDirty())
	ComputeRadii(r_pole, separation, mass_ratio, asynchronism_ratio);

			//if(mParams["g_pole"].isDirty() || mParams["r_pole"].isDirty() || mParams["omega_rot"].isDirty())
	ComputeGravity(g_pole, r_pole, separation, mass_ratio, asynchronism_ratio);

	bool feature_dirty = false;
	for(auto feature: mFeatures)
		feature_dirty |= feature->isDirty();

	if(feature_dirty || mParams["T_eff_pole"].isDirty() || mParams["g_pole"].isDirty() || mParams["von_zeipel_beta"].isDirty())
		VonZeipelTemperatures(T_eff_pole, g_pole, von_zeipel_beta);

	for(auto feature: mFeatures)
		feature->apply(this);

	double max_temperature = 0;
	for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
	{
		if(mPixelTemperatures[i] > max_temperature)
			max_temperature = mPixelTemperatures[i];
	}

	TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_temperature);

	GenerateVBO(n_pixels, n_sides, mVBOData);

	mat4 scale = glm::scale(mat4(), glm::vec3(1, 1, 1));

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

/// Computes the geometry of the spherical Healpix surface
void CRocheLobe::GenerateModel(vector<vec3> & vbo_data,
		vector<unsigned int> & elements)
{
	const double g_pole = mParams["g_pole"].getValue();
	const double r_pole = mParams["r_pole"].getValue();
	const double T_eff_pole = mParams["T_eff_pole"].getValue();
	const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();
	const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());
	const double separation = mParams["separation"].getValue();
	const double mass_ratio = mParams["mass_ratio"].getValue();
	const double asynchronism_ratio = mParams["asynchronism_ratio"].getValue();

	// Generate a unit Healpix sphere
	GenerateHealpixSphere(n_pixels, n_sides);

	// recomputing the sphereoid is very expensive. Make use of the dirty flags
	// to only compute that which is absolutely necessary.
	ComputeRadii(r_pole, separation, mass_ratio, asynchronism_ratio);
	ComputeGravity(g_pole, r_pole, separation, mass_ratio, asynchronism_ratio);
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
