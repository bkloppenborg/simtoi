/* CDensityDisk.h
 *
 *  Created on: Nov. 5, 2013
 *      Author: bkloppenborg
 */

#include "CDensityDisk.h"
#include "CShaderFactory.h"

CDensityDisk::CDensityDisk(int n_additional_params)
: 	CModel(n_additional_params + 4)
{
	// give this object a name
	mName = "Density disk base class";

	// init members:
	mSlices = 50;	// seems like a good number.
	mStacks = 100;
	mSinT = new double[mSlices + 1];
	mCosT = new double[mSlices + 1];
	CModel::CircleTable(mSinT, mCosT, mSlices);

	// Define the placement of the density disk model parameters
	mParamNames.push_back("r_in");
	SetParam(mBaseParams + 1, 0.1);
	SetFree(mBaseParams + 1, false);
	SetMax(mBaseParams + 1, 10);
	SetMin(mBaseParams + 1, 0.1);

	mParamNames.push_back("radial cutoff");
	SetParam(mBaseParams + 2, 20);
	SetFree(mBaseParams + 2, false);
	SetMax(mBaseParams + 2, 10);
	SetMin(mBaseParams + 2, 0.1);

	mParamNames.push_back("height cutoff");
	SetParam(mBaseParams + 3, 5);
	SetFree(mBaseParams + 3, false);
	SetMax(mBaseParams + 3, 10);
	SetMin(mBaseParams + 3, 0.1);

	mParamNames.push_back("n rings (int)");
	SetParam(mBaseParams + 4, 50);
	SetFree(mBaseParams + 4, false);
	SetMax(mBaseParams + 4, 100);
	SetMin(mBaseParams + 4, 1);

	// This model ALWAYS uses the default (pass-through) shader.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("default");
}

CDensityDisk::~CDensityDisk()
{
	delete[] mSinT;
	delete[] mCosT;
}

/// Draws a flat (planar) ring between r_in and r_out.
void CDensityDisk::DrawDisk(double r_in, double r_out)
{
	// lookup constants
	double color = mParams[3];

	// Compute the transparency at the inner/out radii
	const double trans_in = Transparency(r_in, 0, 0);
	const double trans_out = Transparency(r_out, 0, 0);

	glBegin(GL_QUAD_STRIP);

	glNormal3d(0.0, 1.0, 0.0);

	for(int j = 0; j <= mSlices; j++ )
	{
		glColor4d(color, 0.0, 0.0, trans_in);
		glVertex3d(mCosT[ j ] * r_in,  0, mSinT[ j ] * r_in);
		glColor4d(color, 0.0, 0.0, trans_out);
		glVertex3d(mCosT[ j ] * r_out, 0, mSinT[ j ] * r_out);
	}

	glEnd();
}

void CDensityDisk::DrawSide(double radius)
{
    double y0 = 0;
    double y1 = 0;
    double yStep = 0;
    double r0 = 0;
    double r1 = 0;
    double trans0 = 0;
    double trans1 = 0;

    // Look up cutoff values
    const double height_cutoff = mParams[mBaseParams + 9];

	double total_height  = height_cutoff * 2;

    double color = mParams[3];

    // Divide the z direction into mStacks equal steps
    // Stop when we get to mScale in height.
    yStep = total_height / (mStacks);

    // Compute the lower and upper bounds
	y0 = -height_cutoff;
	y1 = y0 + yStep;

	// Compute the lower and upper transparency
	trans0 = Transparency(radius, y0, 0);
	trans1 = Transparency(radius, y1, 0);

	while(y0 < height_cutoff)
	{
		// Draw the top half
		glBegin( GL_QUAD_STRIP );
		for(int j = 0; j <= mSlices; j++ )
		{
			glNormal3d( mCosT[ j ], 0.0, mSinT[ j ]);
			glColor4d(color, 0.0, 0.0, trans0);
			glVertex3d( mCosT[ j ] * radius,  y0, mSinT[ j ] * radius  );
			glColor4d(color, 0.0, 0.0, trans1);
			glVertex3d( mCosT[ j ] * radius,  y1, mSinT[ j ] * radius  );
		}
		glEnd( );

		// Move the lower bound/transparency to the upper bound/transparency
		y0 = y1;
		trans0 = trans1;

		// Update the upper bound/transparency
		y1 += yStep;
		trans1 = Transparency(radius, y1, 0);
	}
}

void CDensityDisk::Render(GLuint framebuffer_object, int width, int height)
{
	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Parameters from CDisk_Pascucci2004:
	const double r_in = mParams[mBaseParams + 1];
	const double r_cutoff  = mParams[mBaseParams + 2];
	const double h_cutoff  = mParams[mBaseParams + 3];
	int n_rings  = ceil(mParams[mBaseParams + 4]);

	if(n_rings < 0)
		n_rings = 1;

	// Determine the step sizes in radius and height
	const double dr = (r_cutoff - r_in) / n_rings;
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

		// Iterate over the remaining rings, filling in the gaps and drawing the vertical wall
		// at the outer-edge of r+dr
		for(double radius = r_in; radius < r_cutoff + dr; radius += dr)
		{
			DrawDisk(radius, radius + dr);
			DrawSide(radius + dr);
		}

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CDensityDisk.Render()");
}

/// Overrides the default CModel::SetShader function.
void CDensityDisk::SetShader(CShaderPtr shader)
{
	// This mode does not accept different shaders, do nothing here.
}

/// Computes the transparency of the disk at the specified radius, height, and
/// wavelength.
double CDensityDisk::Transparency(double radius, double height, double wavelength)
{
	// Look up the density and opacity:
	double rho = Density(radius, height);
	double kappa = Opacity(radius, height, wavelength);

	// compute the transparency and return it.
	return 1 - exp(-1 * kappa * rho);
}
