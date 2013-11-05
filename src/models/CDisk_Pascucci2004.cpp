/*
 * CDisk_ScaleHeightRings.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 */

#include "CDisk_Pascucci2004.h"
#include "CShaderFactory.h"

CDisk_Pascucci2004::CDisk_Pascucci2004()
: 	CDensityDisk(6)
{
	mName = "Flared Disk (Pascucci 2004)";

	// First four parameters are inherited from CDensityDisk:
	// r_in
	// radial cutoff
	// height cutoff
	// n rings (int)

	mParamNames.push_back("rho_0");
	SetParam(mBaseParams + 5, 1);
	SetFree(mBaseParams + 5, false);
	SetMax(mBaseParams + 5, 100);
	SetMin(mBaseParams + 5, 0.1);

	mParamNames.push_back("kappa");
	SetParam(mBaseParams + 6, 3.0);
	SetFree(mBaseParams + 6, false);
	SetMax(mBaseParams + 6, 10);
	SetMin(mBaseParams + 6, 0.1);

	mParamNames.push_back("r0 (scale radius)");
	SetParam(mBaseParams + 7, 1.0);
	SetFree(mBaseParams + 7, true);
	SetMax(mBaseParams + 7, 6.0);
	SetMin(mBaseParams + 7, 0.1);

	mParamNames.push_back("h0 (scale height)");
	SetParam(mBaseParams + 8, 2.0);
	SetFree(mBaseParams + 8, true);
	SetMax(mBaseParams + 8, 10);
	SetMin(mBaseParams + 8, 0.1);

	mParamNames.push_back("alpha (radial power)");
	SetParam(mBaseParams + 9, 1);
	SetFree(mBaseParams + 9, true);
	SetMax(mBaseParams + 9, 10);
	SetMin(mBaseParams + 9, 0.1);

	mParamNames.push_back("beta (height power)");
	SetParam(mBaseParams + 10, 1.25);
	SetFree(mBaseParams + 10, true);
	SetMax(mBaseParams + 10, 10);
	SetMin(mBaseParams + 10, 0.1);

	// This model ALWAYS uses the default (pass-through) shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("default");
}

CDisk_Pascucci2004::~CDisk_Pascucci2004()
{
	// TODO Auto-generated destructor stub
}

shared_ptr<CModel> CDisk_Pascucci2004::Create()
{
	return shared_ptr<CModel>(new CDisk_Pascucci2004());
}

/// Computes the optical depth following the density convention in Pascucci 2004
/// multiplied by the opacity.
///
/// The density function in Pascucci 2004 is:
///  \rho(r,z) = \rho_0 \left( \frac{r}{r_0}\right )^{-\alpha} \exp{\left[-\frac{1}{2} \left( \frac{z}{h_0 (r/r_0)^{\beta}} \right)^{2} \right ]}
/// and this is thrown in an exponential function exp(kappa * rho(r,z)) to yield
/// the optical depth.
double CDisk_Pascucci2004::Density(double radius, double height)
{
	// Look up constants
	const double rho0  = mParams[mBaseParams + 5];
	const double r0 = mParams[mBaseParams + 7];
	const double h0 = mParams[mBaseParams + 8];
	const double alpha = mParams[mBaseParams + 9];
	const double beta = mParams[mBaseParams + 10];

	double midplane_density = rho0 * pow(radius / r0, -alpha);
	double height_scaling = -0.5 * pow(height / (h0 * pow(radius / r0, beta)), 2);

	// compute the optical depth:
	return rho0 * pow(radius / r0, -alpha) * exp(height_scaling);
}

double CDisk_Pascucci2004::Opacity(double radius, double height, double wavelength)
{
	// look up and return the (constant) opacity
	double kappa = mParams[mBaseParams + 6];
	return kappa;
}
