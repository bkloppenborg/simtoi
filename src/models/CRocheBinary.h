/*
 * CRocheBinary.h
 *
 *  Created on: Feb 28, 2014
 *      Author: fbaron
 */

#ifndef CROCHEBINARY_H_
#define CROCHEBINARY_H_

#include "CHealpixSpheroid.h"

class CRocheBinary: public CHealpixSpheroid
{
public:
	CRocheBinary();
	virtual ~CRocheBinary();

public:

	void Render(const glm::mat4 & view);

	static shared_ptr<CModel> Create();

	virtual string GetID()
	{
		return "roche_binary";
	};

private:
	// Constants
	const double AU = 1.496e11; // meters
	const double rsun = 6.955e8; // m	
	const double G = 6.67428e-11; // m3 kg-1 s-2
	const double parsec = 3.08567758e16; // m
	const double msun = 1.9891e30; // kg
	const double gmr = G * msun / rsun;
	const double gmr2 = gmr / rsun;

	// Main parameters
	double lambda; // wavelength of observation, used to convert temperatures to fluxes, in meters
	double parallax; // distance from the sun, in pc
	double orbital_period; // days, orbital period of the star
	double rotation_period; // days, rotation period of the star
	double radius_pole; //  rsun, polar radius of the star
	double desired_resolution; // mas, Healpix resolution
	double gravity_darkening;

	double mass1; // mass of the Roche-lobed star
	double mass2; // mass of the other star in the binary
	double teff_pole; // temperature of the Roche-lobed star

	double massratio;
	double omega_rot; // orbital period in Hz

	double separation_rsun;

	double npix_estimate;
	double nside_estimate;


	// Spots (should be a class, really)
//	int nspots; // number of spots
//	double* spot_temperature;
//	double* spot_theta;
//	double* spot_phi;
//	double* spot_thetasize;
//	double* spot_phisize;

	//void normalize_vert(double * vec);
	void triaxial_pot(double & pot, double & dpot, const double radius,
			const double theta, const double phi);
	void triaxial_gravity(const double radius,
			const double theta, const double phi,
			double & g_x, double & g_y, double & g_z, double & g_mag);
	void ComputeRadii(double* radii, const double *theta, const double *phi,
			const unsigned int vsize);
	void surface_gravity(const double * radii, const double * theta, const double * phi,
			double * g_x, double * g_y, double * g_z, double * g_mag,
			const unsigned int vsize);
	void surface_temperature(double* temperature, const double* gravity,
			const double gravity_pole, const unsigned int vsize);
public:
	void GenerateModel(vector<vec3> & vbo_data,
			vector<unsigned int> & elements);

};

#endif /* CROCHEBINARY_H_ */
