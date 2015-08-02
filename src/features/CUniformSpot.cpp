/*
 * CUniformSpot.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: bkloppenborg
 */

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795028841968
#endif // M_PI
#endif // PI

#include "CUniformSpot.h"
#include "CModel.h"
#include <iostream>

CUniformSpot::CUniformSpot():
	CFeature()
{
	mID = "uniform_spot";
	mName = "Uniform Spot";

	addParameter("theta", 0, 0, 180, false, 0.1, "Theta", "Polar angular position of the spot on the surface as measured from the pole (degrees).");
	addParameter("phi", 0, 0, 180, false, 0.1, "Phi", "Azimuthal angular position of the spot on the surface (degrees).");
	addParameter("radius", 1.0, 0, 10, false, 1, "Radius", "Radius of the spot on the surface  (degrees)");
	addParameter("delta_T", -500, -2000, 2000, false, 100, "Delta T", "Temperature of the spot relative to the surface temperature");
}

CUniformSpot::~CUniformSpot()
{
	// TODO Auto-generated destructor stub
}

/// Applies the uniform spot to the specified model
void CUniformSpot::apply(CModel * model)
{
	// init locals. Convert (user specified) degrees to radians
	const double theta = mParams["theta"].getValue() * PI / 180;
	const double phi = mParams["phi"].getValue() * PI / 180;
	const double spot_radius = mParams["radius"].getValue() * PI / 180;
	const double spot_delta_temperature = mParams["delta_T"].getValue();
	vector<unsigned int> pixel_ids;

	// Get a reference to the object's temperature grid
	vector<double> & temperatures = model->GetPixelTemperatures();

	// Get the indicies for the pixels at (theta, phi) within radius units away.
	model->FindPixels(0, theta, phi, 0, spot_radius, spot_radius, pixel_ids);

	for(auto pixel_id: pixel_ids)
	{
		temperatures[pixel_id] += spot_delta_temperature;
	}
}
