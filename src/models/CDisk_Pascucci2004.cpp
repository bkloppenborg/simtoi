/*
 * CDisk_ScaleHeightRings.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 */

#include "CDisk_Pascucci2004.h"
#include "CShaderFactory.h"

CDisk_Pascucci2004::CDisk_Pascucci2004()
: 	CCylinder(9 - mDiskParams)
{
	mName = "Flared Disk (Pascucci 2004)";

	// WARNING: This class explicitly overrides the default parameters in CModelDisk::InitMembers
	for(int i = 0; i < 2; i++)
		mParamNames.pop_back();

	// Set the members to something useful:
	mParamNames.push_back("rho_0");
	SetParam(mBaseParams + 1, 1);
	SetFree(mBaseParams + 1, false);
	SetMax(mBaseParams + 1, 100);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("kappa");
	SetParam(mBaseParams + 2, 3.0);
	SetFree(mBaseParams + 2, false);
	SetMax(mBaseParams + 2, 10);
	SetMin(mBaseParams + 2, 0.1);

	mParamNames.push_back("r0 (scale radius)");
	SetParam(mBaseParams + 3, 1.0);
	SetFree(mBaseParams + 3, true);
	SetMax(mBaseParams + 3, 6.0);
	SetMin(mBaseParams + 3, 0.1);

	mParamNames.push_back("h0 (scale height)");
	SetParam(mBaseParams + 4, 2.0);
	SetFree(mBaseParams + 4, true);
	SetMax(mBaseParams + 4, 10);
	SetMin(mBaseParams + 4, 0.1);

	mParamNames.push_back("alpha (radial power)");
	SetParam(mBaseParams + 5, 1);
	SetFree(mBaseParams + 5, true);
	SetMax(mBaseParams + 5, 10);
	SetMin(mBaseParams + 5, 0.1);

	mParamNames.push_back("beta (height power)");
	SetParam(mBaseParams + 6, 1.25);
	SetFree(mBaseParams + 6, true);
	SetMax(mBaseParams + 6, 10);
	SetMin(mBaseParams + 6, 0.1);

	mParamNames.push_back("radial cutoff");
	SetParam(mBaseParams + 7, 20);
	SetFree(mBaseParams + 7, false);
	SetMax(mBaseParams + 7, 10);
	SetMin(mBaseParams + 7, 0.1);

	mParamNames.push_back("height cutoff");
	SetParam(mBaseParams + 8, 5);
	SetFree(mBaseParams + 8, false);
	SetMax(mBaseParams + 8, 10);
	SetMin(mBaseParams + 8, 0.1);

	mParamNames.push_back("n rings (int)");
	SetParam(mBaseParams + 9, 50);
	SetFree(mBaseParams + 9, false);
	SetMax(mBaseParams + 9, 100);
	SetMin(mBaseParams + 9, 1);

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
///  \rho(r,z) = \rho_0  \left( \frac{r}{r_0}\right )^{-\alpha} \exp{\left[-\frac{1}{2} \left( \frac{z}{h_0 (r/r_0)^{\beta} \right)^{2}} \right ]}
/// and this is thrown in an exponential function exp(kappa * rho(r,z)) to yield
/// the optical depth.
double CDisk_Pascucci2004::Density(double radius, double height)
{
	// Look up constants
	const double rho0  = mParams[mBaseParams + 1];
	const double r0 = mParams[mBaseParams + 3];
	const double h0 = mParams[mBaseParams + 4];
	const double alpha = mParams[mBaseParams + 5];
	const double beta = mParams[mBaseParams + 6];

	double midplane_density = rho0 * pow(radius / r0, -alpha);
	double height_scaling = -0.5 * pow(height / (h0 * pow(radius / r0, beta)), 2);

	// compute the optical depth:
	return rho0 * pow(radius / r0, -alpha) * exp(height_scaling);
}

/// Draws a flat (planar) ring between r_in and r_out with the specified transparency.
void CDisk_Pascucci2004::DrawDisk(double r_in, double r_out, double transparency)
{
    double color = mParams[3];
    glBegin(GL_QUAD_STRIP);

		glNormal3d(0.0, 1.0, 0.0);

		for(int j = 0; j <= mSlices; j++ )
		{
			glColor4d(color, 0.0, 0.0, transparency);
			glVertex3d(mCosT[ j ] * r_in,  0, mSinT[ j ] * r_in);
			glVertex3d(mCosT[ j ] * r_out, 0, mSinT[ j ] * r_out);
		}

	glEnd();
}

void CDisk_Pascucci2004::DrawSide(double radius)
{
	double transparency = 0;
    double y0 = 0;
    double y1 = 0;
    double yStep = 0;
    double r0 = 0;
    double r1 = 0;

    // Compute the cutoff location
    double height_cutoff = mParams[mBaseParams + 8];
	double total_height  = height_cutoff * 2;

    double color = mParams[3];

    // Divide the z direction into mStacks equal steps
    // Stop when we get to mScale in height.
    yStep = total_height / mStacks;

	y0 = -height_cutoff;
	y1 = y0 + yStep;

	while(y0 < height_cutoff)
	{
		glColor4d(color, 0.0, 0.0, Transparency(radius, y0, 0));

		// Draw the top half
		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ],     0.0, mSinT[ j ]       );
			glVertex3d( mCosT[ j ] * radius,  y0, mSinT[ j ] * r0  );
			glVertex3d( mCosT[ j ] * radius,  y1, mSinT[ j ] * r1  );
		}
		glEnd( );

		y0 = y1;
		y1 += yStep;
	}
}

double CDisk_Pascucci2004::Opacity(double radius, double height, double wavelength)
{
	// look up and return the (constant) opacity
	double kappa = mParams[mBaseParams + 2];
	return kappa;
}

void CDisk_Pascucci2004::Render(GLuint framebuffer_object, int width, int height)
{
	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Parameters from CDisk_Pascucci2004:
	const double rho0  = mParams[mBaseParams + 1];
	const double kappa = mParams[mBaseParams + 2];
	const double r0 = mParams[mBaseParams + 3];
	const double h0 = mParams[mBaseParams + 4];
	const double alpha = mParams[mBaseParams + 5];
	const double beta = mParams[mBaseParams + 6];
	const double r_cutoff  = mParams[mBaseParams + 7];
	const double h_cutoff  = mParams[mBaseParams + 8];
	int n_rings  = ceil(mParams[mBaseParams + 9]);

	if(n_rings < 0)
		n_rings = 1;

	// Determine the step sizes in radius and height
	const double dr = (r_cutoff) / n_rings;
	const double dh = (h_cutoff) / mStacks;

	// Set some (bogus) values for the shader
	double min_xyz[3] = {0, 0, 0};
	double max_xyz[3] = {0, 0, 0};

	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
		SetupMatrix();
		Color();
		UseShader(min_xyz, max_xyz);

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		// Draw the inner wall first
		//DrawSides(r_in, total_height);

		// Iterate over the remaining rings, filling in the gaps and drawing the vertical wall
		// at the outer-edge of r+dr
		for(double radius = 0; radius < r_cutoff + dr; radius += dr)
		{
			DrawDisk(radius, radius + dr, Transparency(radius, 0, 0));
			DrawSide(radius + dr);
		}

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CModelDisk.Render()");
}

/// Overrides the default CModel::SetShader function.
void CDisk_Pascucci2004::SetShader(CShaderPtr shader)
{
	// This mode does not accept different shaders, do nothing here.
}

/// Computes the transparency of the disk at the specified radius, height, and wavelength.
///
double CDisk_Pascucci2004::Transparency(double radius, double height, double wavelength)
{
	// Look up the density and opacity:
	double rho = Density(radius, height);
	double kappa = Opacity(radius, height, wavelength);

	// compute the transparency and return it.
	return 1 - exp(-1 * kappa * rho);
}
