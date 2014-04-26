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
	CFeature(4)
{
	mName = "Uniform Spot";

	// coordinate on surface (degrees)
	mParamNames.push_back("theta");
	SetParam(0, 0.0);
	SetFree(0, false);
	SetMax(0, 180);
	SetMin(0, 0.1);

	// coordinate on surface (degrees)
	mParamNames.push_back("phi");
	SetParam(1, 0.0);
	SetFree(1, false);
	SetMax(1, 360);
	SetMin(1, 0.0);

	// radius of spot (degrees)
	mParamNames.push_back("radius");
	SetParam(2, 1.0);
	SetFree(2, false);
	SetMax(2, 6.0);
	SetMin(2, 0.1);

	// diameter of spot
	mParamNames.push_back("spot_delta_temperature");
	SetParam(3, -500);
	SetFree(3, false);
	SetMax(3, 2000);
	SetMin(3, -2000.0);
}

CUniformSpot::~CUniformSpot()
{
	// TODO Auto-generated destructor stub
}

/// Applies the uniform spot to the specified model
void CUniformSpot::apply(CModel * model)
{
	// init locals. Convert (user specified) degrees to radians
	const double theta = mParams[0] * PI / 180;
	const double phi = mParams[1] * PI / 180;
	const double spot_radius = mParams[2] * PI / 180;
	const double spot_delta_temperature = mParams[3];
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
