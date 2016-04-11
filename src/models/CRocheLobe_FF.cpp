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
 *  Copyright (c) 2014 Fabien Baron
 *
 *  Description:
 *  basic Roche lobe with co-aligned self-rotation and orbital rotation
 *  and non-synchronous rotation periods
 *
 *  Note:   most of the code is not optimized at the moment...
 */

#include "CShaderFactory.h"
#include "CFeature.h"
#include "CRocheLobe_FF.h"

CRocheLobe_FF::CRocheLobe_FF() : AU(1.496e11), rsun(6.955e8), G(6.67428e-11), parsec(3.08567758e16), CHealpixSpheroid()
{
    mID = "roche_lobe_FF";
    mName = "Roche Lobe (Fill Factor)";

    addParameter("T_eff_pole", 5000, 2E3, 1E6, false, 100, "T_pole", "Effective Polar temperature (kelvin)", 0);
    addParameter("von_zeipel_beta", 0.25, 0.0, 1.0, false, 0.1, "Beta", "Von Zeipel gravity darkening parameter (unitless)", 2);
    addParameter("F", 0.5 , 0.001, 1.0, false, 0.01, "Fill factor", "Fill factor, as defined by Mochnacki", 2);
    addParameter("separation", 4.0 , 0.1, 100.0, false, 0.01, "Separation", "Separation between components (mas)", 2);
    addParameter("q", 3.0 , 0.001, 100.0, false, 0.01, "Mass ratio", "M2/M1 mass ratio; M1 = this Roche lobe (unitless)", 2);
    addParameter("P", 1.0 , 0.01, 2.0, false, 0.01, "Async ratio", "Ratio self-rotation period/orbital revolution period (unitless)", 2);
    //	omega_rot = 2.0 * PI / (orbital_period * 3600. * 24.); // in Hz
}

CRocheLobe_FF::~CRocheLobe_FF()
{

}

shared_ptr<CModel> CRocheLobe_FF::Create()
{
    return shared_ptr < CModel > (new CRocheLobe_FF());
}

void CRocheLobe_FF::ComputeRadii(const double pot_surface, const double separation, const double q, const double P)
{
    // Compute the radii for the pixels and corners:
    for(unsigned int i = 0; i < pixel_radii.size(); i++)
        pixel_radii[i] = ComputeRadius(pot_surface, separation, q, P, pixel_theta[i], pixel_phi[i]);

    for(unsigned int i = 0; i < corner_radii.size(); i++)
        corner_radii[i] = ComputeRadius(pot_surface, separation, q, P, corner_theta[i], corner_phi[i]);
}

void CRocheLobe_FF::ComputeGravity(const double separation, const double q, const double P)
{
    // Compute the gravity vector for each pixel:
    for(unsigned int i = 0; i < gravity.size(); i++)
    {
        ComputeGravity(separation, q, P, pixel_radii[i], pixel_theta[i], pixel_phi[i], g_x[i], g_y[i], g_z[i], gravity[i]);
    }
}

void CRocheLobe_FF::VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta)
{
    for(unsigned int i = 0; i < mPixelTemperatures.size(); i++)
        mPixelTemperatures[i] = T_eff_pole * pow(gravity[i] / g_pole, beta);
}

/// Computes the tangential components and magnitude of gravity at the
/// specified (r, theta, phi) coordinates.
void CRocheLobe_FF::ComputeGravity(const double separation, const double q, const double P, const double radius, const double theta, const double phi, double & g_x, double & g_y, double & g_z, double & g_mag)
{
    double radius1 = radius / separation;  // dimensionless
    double l = cos(phi) * sin(theta);
    double mu = sin(phi) * sin(theta);
    double x = radius1 * l;
    double y = radius1 * mu;
    double z = radius1 * cos(theta);
    double radius2 = std::sqrt( (x - 1.0) * (x - 1.0) + y * y + z * z);
    double radius1_pow3 = radius1 * radius1 * radius1;
    double radius2_pow3 = radius2 * radius2 * radius2;

    // gx, gy, gz are the cartesian coordinates of the gravity vector
    // we need them in cartesian form to compute the vertices for limb-darkening

    g_x = - x / radius1_pow3 + q * (1.0 - x) / radius2_pow3 + P * P * (1 + q) * x - q;
    g_y = - y / radius1_pow3 - q * y / radius2_pow3 + P * P * (1 + q) * y;
    g_z = - z / radius1_pow3 - q * z / radius2_pow3;
    g_mag = std::sqrt(g_x * g_x + g_y * g_y + g_z * g_z);
}


void CRocheLobe_FF::ComputePotential(double & pot, double & dpot, const double radius, const double theta, const double phi, const double separation, const double q, const double P)
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
    //const double xc =  q / (1. + q);
    const double radius2 = std::sqrt( (x - 1.0) * (x - 1.0) + y * y + z * z);

    pot = - 1.0 / radius1 - q / radius2  + q * x - 0.5 * (q + 1.0) * P * P
        * (x * x + y * y );

    dpot =  1.0 / (radius1 * radius1) + q / (radius2 * radius2 *radius2) * (radius1 - l)
        + q * l - (q + 1.0) * P * P * radius1 * ( l * l + mu * mu);

}


double CRocheLobe_FF::ComputeRadius(const double pot_surface, const double separation, const double q, const double P, const double theta, const double phi)
{
    // in this function we compute the roche radius based on masses/ distance / orbital_period, for each (theta, phi)
    const double epsilon = 1e-2;
    register int i;
    double pot, dpot;
    double newton_step;

    // Unlike the roche lobe based on polar radius, here we use directly the potential derived from fill factor
    double radius = 0.25*separation; // initial guess for the radius, TBD: improve !
    bool converged = false;
    for(int i=0; i<10;i++)//while(converged == FALSE)
    {
        ComputePotential(pot, dpot, radius, theta, phi, separation, q, P );
        newton_step = (pot - pot_surface) / dpot; // newton step
        radius = radius * (1. - newton_step);

        if (fabs(newton_step) < epsilon)
            converged = true;
    }
    return radius;
}


void CRocheLobe_FF::preRender(double & max_flux)
{
    if (!mModelReady)
        Init();

    // See if the user change the tesselation
    const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());
    if(mParams["n_side_power"].isDirty())
    {
        Init();
    }

    const double T_eff_pole = mParams["T_eff_pole"].getValue();
    const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();
    const double separation = mParams["separation"].getValue();
    const double q = mParams["q"].getValue();
    const double P = mParams["P"].getValue();
    const double F = mParams["F"].getValue();

    //printf("%lf %lf %lf %lf", q, P, F, separation);
    // First find the position of L1 point on the axis
    double r_L1 = separation * ComputeRL1(q, P);
    // Potential at L1 point
    double pot_L1, dpot_L1;
    ComputePotential(pot_L1, dpot_L1, r_L1, PI/2., 0.0, separation, q, P);
    // Potential at F surface
    double pot_surface = (pot_L1 + 0.5*q*q/(1.+q))/F - 0.5*q*q/(1.+q);

    // Compute r_pole
    double r_pole = ComputeRadius(pot_surface, separation, q, P, 0.0, 0.0);

    // Compute gravity at pole
    double g_pole, tempx, tempy, tempz;
    ComputeGravity(separation, q, P, r_pole, 0.0, 0.0, tempx, tempy, tempz, g_pole);

    // Compute Radii for all Healpix pixels
    ComputeRadii(pot_surface, separation, q, P);
    // Compute Gravity for all Healpix pixels
    ComputeGravity(separation, q, P);

    bool feature_dirty = false;
    for(auto feature: mFeatures)
        feature_dirty |= feature->isDirty();

    if(feature_dirty || mParams["T_eff_pole"].isDirty() || mParams["von_zeipel_beta"].isDirty())
        VonZeipelTemperatures(T_eff_pole, g_pole, von_zeipel_beta);

    for(auto feature: mFeatures)
        feature->apply(this);


    TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_flux);

    GenerateVBO(n_pixels, n_sides, mVBOData);

}

void CRocheLobe_FF::Render(const glm::mat4 & view, const GLfloat & max_flux)
{
    const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());

    NormalizeFlux(max_flux);

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
void CRocheLobe_FF::GenerateModel(vector<vec3> & vbo_data,
        vector<unsigned int> & elements)
{
    const double T_eff_pole = mParams["T_eff_pole"].getValue();
    const double von_zeipel_beta = mParams["von_zeipel_beta"].getValue();
    const unsigned int n_sides = pow(2, mParams["n_side_power"].getValue());
    const double separation = mParams["separation"].getValue();
    const double q = mParams["q"].getValue();
    const double P = mParams["P"].getValue();
    const double F = mParams["F"].getValue();
    // Generate a unit Healpix sphere
    GenerateHealpixSphere(n_pixels, n_sides);

    // recomputing the sphereoid is very expensive. Make use of the dirty flags
    // to only compute that which is absolutely necessary.
    double r_L1 = separation * ComputeRL1(q, P);
    // Potential at L1 point
    double pot_L1, dpot_L1;
    ComputePotential(pot_L1, dpot_L1, r_L1, PI/2., 0.0, separation, q, P);
    // Potential at F surface
    double pot_surface = (pot_L1 + 0.5*q*q/(1.+q))/F - 0.5*q*q/(1.+q);
    // Compute r_pole
    double r_pole = ComputeRadius(pot_surface, separation, q, P, 0.0, 0.0);
    // Compute gravity at pole
    double g_pole, tempx, tempy, tempz;
    ComputeGravity(separation, q, P, r_pole, 0.0, 0.0, tempx, tempy, tempz, g_pole);
    // Compute Radii for all Healpix pixels
    ComputeRadii(pot_surface, separation, q, P);
    // Compute Gravity for all Healpix pixels
    ComputeGravity(separation, q, P);
    // Compute Temperatures for all Healpix pixels
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

double CRocheLobe_FF::ComputeRL1(const double q, const double P)
{ // note: this returns dimensionless rL1, i.e. true_rL1 = rL1*separation
    double xa, xb, xc, pot, dpota, dpotb, dpotc;
    double xd, xs, dpots, dum;
    bool mflag, con1, con2, con3, con4, con5;
    // convergence criterion in x
    const double x_tol = 1e-7;
    // tolerance
    const double dpot_tol = 1e-7;
    // initial bracket values
    xa = 1.e-4;
    xb = 0.9999;
    // compute dimensionless rL1 (separation = 1)
    ComputePotential(pot, dpota, xa, PI/2, 0, 1.0, q, P);
    ComputePotential(pot, dpotb, xb, PI/2, 0, 1.0, q, P);
    // test that root is bracketed by a,b
    if (dpota * dpotb >= 0)
    {
        printf("Issue bracketing rL1 position");
        // TODO: handle exception gracefully here
    }
    //swap a,b if needed
    if ( abs(dpota) < abs(dpotb))
    {
        dum = xa; xa = xb; xb = dum; dum = dpota; dpota = dpotb; dpotb = dum;
    }

    xc = xa;
    ComputePotential(pot, dpotc, xc, PI/2, 0, 1.0, q, P);
    mflag = TRUE;

    while ((abs(xb-xa) > x_tol) && ( abs(dpotb) > dpot_tol))
    {
        if ( (abs(dpota-dpotc) > dpot_tol) && (abs(dpotb - dpotc) > dpot_tol) )
        {
            xs = xa * dpotb * dpotc / ( (dpota-dpotb) * (dpota-dpotc) ) + xb * dpota * dpotc / ( (dpotb-dpota) * (dpotb-dpotc)) + xc * dpota * dpotb / ( (dpotc-dpota) * (dpotc-dpotb) );
        }
        else
        {
            xs=xb-dpotb*(xb-xa)/(dpotb-dpota);
        }
        con1 = (xs <= (3.0*xa+xb)/4.0) || (xs >= xb);
        con2 = mflag && (abs(xs-xb) >= 0.5*abs(xb-xc) );
        con3 = (!mflag) && (abs(xs-xb) >= 0.5*abs(xc-xd));
        con4 = mflag && (abs(xb-xc) < abs(x_tol));
        con5 = (!mflag) && (abs(xc-xd) < abs(x_tol));
        if (con1 || con2 || con3 || con4 || con5)
        {
            xs=(xa+xb)/2.0;
            mflag=TRUE;
        }
        else mflag= FALSE;

        ComputePotential(pot, dpots, xs, PI/2, 0, 1.0, q, P);
        xd=xc;
        xc=xb;
        ComputePotential(pot, dpotc, xc, PI/2, 0, 1.0, q, P);

        if (dpota*dpots < 0.)
        {
            xb=xs;
            dpotb=dpots;
        }
        else
        {
            xa=xs;
            dpota=dpots;
        }

        // swap a,b if needed
        if (abs(dpota) < abs(dpotb))
        {
            dum = xa; xa = xb; xb = dum;
            dum = dpota; dpota = dpotb; dpotb = dum;
        }
    }
    //printf("rL1: %lf", xs);
    return xs;
}
