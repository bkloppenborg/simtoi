/*
 * CRocheBinary.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: fbaron
 *      Description: basic stellar shape based on Roche equations, with gravity darkening
 *      Note:   most of the code is not optimized at the moment...
 * 
 */

#include "CRocheBinary.h"
#include "CShaderFactory.h"

CRocheBinary::CRocheBinary() :
	CHealpixSpheroid()
{

//	mParamNames.push_back("Wavelength (um)");
//	SetParam(mBaseParams + 1,  1.4);
//	SetFree(mBaseParams + 1, false);
//	SetMax(mBaseParams + 1, 5);
//	SetMin(mBaseParams + 1, 0.1);
//
//	mParamNames.push_back("Parallax (mas)");
//	SetParam(mBaseParams + 2, 28.8);
//	SetFree(mBaseParams + 2, false);
//	SetMax(mBaseParams + 2, 0.1);
//	SetMin(mBaseParams + 2, 50);
//
//	mParamNames.push_back("Orbital Period (d)");
//	SetParam(mBaseParams + 3, 2.87);
//	SetFree(mBaseParams + 3, false);
//	SetMax(mBaseParams + 3, 0.1);
//	SetMin(mBaseParams + 3, 50);
//
//	mParamNames.push_back("Rotation Period (d)");
//	SetParam(mBaseParams + 4, 2.87);
//	SetFree(mBaseParams + 4, false);
//	SetMax(mBaseParams + 4, 0.1);
//	SetMin(mBaseParams + 4, 50);
//
//	mParamNames.push_back("Healpix Resolution (mas)");
//	SetParam(mBaseParams + 5, 0.05);
//	SetFree(mBaseParams + 5, false);
//	SetMax(mBaseParams + 5, 0.01);
//	SetMin(mBaseParams + 5, 5.0);
//
//	mParamNames.push_back("Mass 1 (MSolar)");
//	SetParam(mBaseParams + 6, 0.75);
//	SetFree(mBaseParams + 6, false);
//	SetMax(mBaseParams + 6, 0.1);
//	SetMin(mBaseParams + 6, 50.0);
//
//	mParamNames.push_back("Mass 2 (MSolar)");
//	SetParam(mBaseParams + 7, 3.2);
//	SetFree(mBaseParams + 7, false);
//	SetMax(mBaseParams + 7, 0.1);
//	SetMin(mBaseParams + 7, 50.0);
//
//	mParamNames.push_back("Polar Temperature (K)");
//	SetParam(mBaseParams + 8, 3700);
//	SetFree(mBaseParams + 8, false);
//	SetMax(mBaseParams + 8, 1500);
//	SetMin(mBaseParams + 8, 25000.0);
//
//	mParamNames.push_back("Polar Radius (RSolar)");
//	SetParam(mBaseParams + 9, 3.25);
//	SetFree(mBaseParams + 9, false);
//	SetMax(mBaseParams + 9, 1.0);
//	SetMin(mBaseParams + 9, 1000);
//
//	mParamNames.push_back("Gravity Darkening");
//	SetParam(mBaseParams + 10, 0.2);
//	SetFree(mBaseParams + 10, false);
//	SetMax(mBaseParams + 10, 0.01);
//	SetMin(mBaseParams + 10, 1.0);

	addParameter("scale", 0.2, 0.01, 10, false, 0.1, "Scale (temp)", "Model Scale (a temporary implementation)");

//	// Fundamental properties of the star
	lambda = 1.4e-6; // m, wavelength of observation, used to convert temperatures to fluxes
	parallax = 28.8; // distance from the sun, in pc
	orbital_period = 2.87; // days
	rotation_period = orbital_period; // days, using synchronous rotation, but can work asynchronously
	desired_resolution = .05; // mas
	mass1 = 0.75; // mass of the Roche-lobed star
	mass2 = 3.2; // mass of the other star in the binary
	teff_pole = 3700.; // temperature of the Roche-lobed star
	radius_pole = 3.25; //  rsun
	gravity_darkening = 0.2;

	// Derived values
	separation_rsun = pow((pow(orbital_period / 365.24, 2) * (mass1 + mass2)),
			1. / 3.) * AU / rsun; // TBD: replace this semimajor axis by the instantaneous separation
	cout << "Estimated semimajor axis: " << separation_rsun << " Rsun \n";
	massratio = mass2 / (mass1 + mass2);
	omega_rot = 2.0 * PI / (rotation_period * 3600. * 24.); // in Hz

	// Setup Healpix and geometry
//	npix_estimate = 4. * PI
//			* pow(radius_pole * rsun / (parallax * AU) * 1000.
//							/ desired_resolution, 2.0);
//	nside_estimate = std::sqrt(npix_estimate / 12.0);
//	n_sides = pow(2, ceil(log(nside_estimate) / log(2.0)));
//	npix = nside2npix(n_sides);
//	cout << "Nside: " << n_sides << "\tNpix: " << npix << "\n";

	// Volume-equivalent Roche lobe radius from Eggleton
	double q = mass1 / mass2;
	double rl_rsun = separation_rsun * 0.49 * pow(q, 2. / 3.)
			/ (0.6 * pow(q, 2. / 3.) + log(1. + pow(q, 1. / 3.)));
	cout << "Rl (rsun): " << rl_rsun << "\n";

	// Setup spots

//	nspots = 0;
	/*
	 spot_theta = new double[nspots];
	 spot_phi = new double[nspots];
	 spot_thetasize = new double[nspots];
	 spot_phisize = new double[nspots];
	 spot_temperature = new double[nspots];

	 spot_theta[0] = 45./180.*PI ;
	 spot_phi[0] = 45./180.*PI ;
	 spot_thetasize[0] = 15./180.*PI ;
	 spot_phisize[0] = 10./180.*PI ;
	 spot_temperature[0] = 7000.;
	 */
}

CRocheBinary::~CRocheBinary()
{

}

shared_ptr<CModel> CRocheBinary::Create()
{
	return shared_ptr < CModel > (new CRocheBinary());
}

/// Computes the tangential components and magnitude of gravity at the
/// specified (r, theta, phi) coordinates.
void CRocheBinary::triaxial_gravity(const double radius,
		const double theta, const double phi,
		double & g_x, double & g_y, double & g_z, double & g_mag)
{
	double x1, x2, x3, y, z, radius1, radius2, radius1_pow3, radius2_pow3, l,
			mu;

	x1 = radius * cos(phi) * sin(theta);
	x2 = separation_rsun - x1;
	y = radius * sin(phi) * sin(theta);
	z = radius * cos(theta);
	radius2 = std::sqrt(x2 * x2 + y * y + z * z);
	radius1_pow3 = radius * radius * radius;
	radius2_pow3 = radius2 * radius2 * radius2;

	// gx, gy, gz are the coordinates of the gravity vector
	g_x = gmr2 * mass1 * x1 / radius1_pow3 - gmr2 * mass2 * x2 / radius2_pow3
			- omega_rot * omega_rot * rsun * (x1 - separation_rsun * massratio);
	g_y = gmr2 * mass1 * y / radius1_pow3 + gmr2 * mass2 * y / radius2_pow3
			- omega_rot * omega_rot * rsun * y;
	g_z = gmr2 * mass1 * z / radius1_pow3 + gmr2 * mass2 * z / radius2_pow3;

	g_mag = std::sqrt(g_x * g_x + g_y * g_y + g_z * g_z);
}

void CRocheBinary::surface_gravity(const double * radii, const double * theta, const double * phi,
		double * g_x, double * g_y, double * g_z, double * g_mag,
		const unsigned int vsize)
{
	for (unsigned int i = 0; i < vsize; i++)
		triaxial_gravity(radii[i], theta[i], phi[i], g_x[i], g_y[i], g_z[i], gravity[i]);
}

void CRocheBinary::surface_temperature(double * temperature,
		const double * gravity, const double gravity_pole,
		const unsigned int vsize)
{
	for (unsigned int i = 0; i < vsize; i++)
		temperature[i] = teff_pole
				* pow(gravity[i] / gravity_pole, gravity_darkening);
}

void CRocheBinary::triaxial_pot(double & pot, double & dpot,
		const double radius, const double theta, const double phi)
{
	// TBD this is using Pringle 1985, or for a more recent ref Regos 2005 (http://adsabs.harvard.edu/abs/2005MNRAS.358..544R )
	// This is only valid for circular + synchronous rotation, so this will be replaced by Sepinsky 2007
	// theta in radians: co-latitude -- vector
	// phi in radians: phi phi=0 points toward companion -- vector
	// vsize = vector size
	double x1, x2, x3, y, z, radius1, radius2, l, mu;
	radius1 = fabs(radius);
	l = cos(phi) * sin(theta);
	mu = sin(phi) * sin(theta);
	x1 = radius1 * l;
	x2 = x1 - separation_rsun;
	x3 = x1 - separation_rsun * massratio;
	y = radius1 * mu;
	z = radius1 * cos(theta);
	radius2 = std::sqrt(x2 * x2 + y * y + z * z);
	pot = -gmr * (mass1 / radius1 + mass2 / radius2)
			- 0.5 * omega_rot * omega_rot * rsun * rsun * (x3 * x3 + y * y);
	dpot = gmr2
			* (mass1 / (radius1 * radius1)
					+ mass2 / (radius2 * radius2 * radius2)
							* (radius1 - separation_rsun * l))
			- omega_rot * omega_rot * rsun
					* (radius1 * l * l - l * separation_rsun * massratio
							+ radius1 * mu * mu);
}

void CRocheBinary::ComputeRadii(double * radii, const double * theta,
		const double * phi, const unsigned int vsize)
{
	// in this function we compute the roche radius based on masses/ distance / orbital_period, for each (theta, phi)
	const double epsilon = 1;
	register int i;
	double pot_surface, pot, dpot;
	double newton_step;

	triaxial_pot(pot_surface, dpot, radius_pole, 0.0, 0.0); // potential at the equator

	for (i = 0; i < vsize; i++)
		radii[i] = 1.22 * radius_pole; // initial guess for the radius

	unsigned short converged[vsize]; // tracks the convergence for each radius
	for (i = 0; i < vsize; i++)
		converged[i] = 0;

	unsigned int all_converged = 0;
	//unsigned int counter =0;

	while (all_converged < vsize - 1)
	{
		all_converged = 0;

		for (i = 0; i < vsize; i++)
		{
			if (converged[i] == 0)
			{
				// we compute only up to the precision for each element, then we skip
				// not sure if this is faster than vectorizing...
				triaxial_pot(pot, dpot, radii[i], theta[i], phi[i]);
				newton_step = (pot - pot_surface) / (rsun * dpot); // newton step
				radii[i] = radii[i] - newton_step;
				if (fabs(newton_step) < epsilon)
				{
					converged[i] = 1;
					all_converged += 1;
				}
				//      counter++;
			}
		}

		cout << "Convergence " << all_converged << "\n";
	}
}

void CRocheBinary::Render(const glm::mat4 & view)
{
	if (!mModelReady)
		Init();

	// See if the user change the tesselation
	const unsigned int n_sides = mParams["n_side_power"].getValue();
	if(mParams["n_side_power"].isDirty())
	{
		Init();
	}

	// Rename a few variables for convenience:
	double radial_scale = mParams["scale"].getValue();
	mat4 scale = glm::scale(mat4(),
			glm::vec3(radial_scale, radial_scale, radial_scale));

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

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Rendering failed");
}

/// Computes the geometry of the spherical Healpix surface
void CRocheBinary::GenerateModel(vector<vec3> & vbo_data,
		vector<unsigned int> & elements)
{
	// Generate a unit Healpix sphere
	const unsigned int n_sides = mParams["n_side_power"].getValue();
	GenerateHealpixSphere(n_pixels, n_sides);

	// Compute geometry of the Roche spheroid

	// Compute the surface radii for each vertices using the angles and Roche potential equations
	ComputeRadii(&pixel_radii[0], &pixel_theta[0], &pixel_phi[0], n_pixels);
	ComputeRadii(&corner_radii[0], &corner_theta[0], &corner_phi[0], 4 * n_pixels);

	// Compute the gravity darkening (based on the center of the Healpix pixel)
	surface_gravity(&pixel_radii[0], &pixel_theta[0], &pixel_phi[0],
			&g_x[0], &g_y[0], &g_z[0], &gravity[0], n_pixels);

	double gravity_pole = 1;
	double g_pole_x, g_pole_y, g_pole_z;
	triaxial_gravity(radius_pole, 0.0, 0.0, g_pole_x, g_pole_y, g_pole_z, gravity_pole);

	surface_temperature(&mPixelTemperatures[0], &gravity[0], gravity_pole, n_pixels);

	double max_temperature = 0;
	for(unsigned int i = 0; i < n_pixels; i++)
	{
		if(mPixelTemperatures[i] > max_temperature)
			max_temperature = mPixelTemperatures[i];
	}

	// Convert temperature into fluxes
	TemperatureToFlux(mPixelTemperatures, mFluxTexture, lambda, max_temperature);

	GenerateVBO(n_pixels, n_sides, vbo_data);

	// Create the lookup indicies.
	GenerateHealpixVBOIndicies(n_pixels, elements);
}
