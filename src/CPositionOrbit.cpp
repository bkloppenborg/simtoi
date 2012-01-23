/*
 * CPositionOrbit.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 */

#include "CPositionOrbit.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795028841968
#endif

CPositionOrbit::CPositionOrbit()
	: CPosition(7)
{
	mType = POSITION_ORBIT;
	mTime = 0;
	mName = "Orbit";

	mParamNames.push_back("Omega");
	mParamNames.push_back("Inc");
	mParamNames.push_back("omega");
	mParamNames.push_back("Alpha");
	mParamNames.push_back("e");
	mParamNames.push_back("tau");
	mParamNames.push_back("T");
	SetParam(6, 1.0);
	SetAllFree(false);
}

CPositionOrbit::~CPositionOrbit()
{
	// TODO Auto-generated destructor stub
}

// Computes the mean angular velocity for the given period, T
double CPositionOrbit::ComputeN(double T)
{
	return 2.0 * PI / T;
}

// Computes the mean anomaly given tau, n, and the time of observation, t.
double CPositionOrbit::ComputeM(double tau, double n, double t)
{
    return n*(t - tau);
}

double CPositionOrbit::ComputeE(double M, double e)
{
    /*
    Solve Kepler's equation, M = E - e sin(E), using a Newton-Raphson method:

    TODO: This function could be rewritten to use Laguerre polynomials:
    http://www.springerlink.com/content/p122000960815647/
    as it has been shown to converge for all e, and faster than the Newton-Raphson
    method used below.

    TODO: Possibly try memoization of of E given (M, e).  Should help speed things up
    a little, especially if M and e are held constant.
    */

    int i = 0;

    //printf("M %f \n", M);
    // Initial guess (from Smith (1979))
    double E = M + e * sin(M) / (1 - sin(M + e) + sin(M));
    double E_old = 0;
    //printf("E0 %f \n", E);

    // Below we are using the Newton-Raphson solution.  The second term is the derivative
    // of Kepler's equation.
    if(e > 0.5)
    {
		do
		{
			i++;
			E_old = E;
			E = E_old + (M + e * sin(E_old) - E_old) / (1 - e * cos(E_old));
			//printf("E %f \n", E);
		}
		while ((i < ORBIT_MAX_ITERATIONS) && ((E - E_old) > ORBIT_THRESH));
    }
    else
    {
		do
		{
			i++;
			E_old = E;
			E = M + e * sin(E);
		} while ((i < ORBIT_MAX_ITERATIONS) && ((E - E_old) > ORBIT_THRESH));
    }

//    if(i == (MAX_ITERATIONS - 1))
//        printf("WARNING: Failed to Solve Kepler's Equation!");

    //printf("Ef %f \n", E);

    return E;
}

// Computes the coefficients, L1, L2, M1, M2, N1, N2 for the orbital equations
void CPositionOrbit::Compute_Coefficients(double Omega, double inc, double omega,
		double & L1, double & M1, double & N1, double & L2, double & M2, double & N2)
{
    double c_Omega = cos(Omega);
    double s_Omega = sin(Omega);
    double c_inc = cos(inc);
    double s_inc = sin(inc);
    double c_omega = cos(omega);
    double s_omega = sin(omega);

    // NOTE: Roy's orbital motion equations assume the ecliptic is at i = 0
    // astronomy doesn't use the same convention so L <-> M compared to Roy.
//    M1 = c_Omega * c_omega - s_Omega * s_omega * c_inc;
//    L1 = s_Omega * c_omega + c_Omega * s_omega * c_inc;
//    N1 = s_omega * s_inc;
//    M2 = -c_Omega * s_omega - s_Omega * c_omega * c_inc;
//    L2 = -s_Omega * s_omega + c_Omega * c_omega * c_inc;
//    N2 = c_omega * s_inc;
    L1 = c_Omega * c_omega - s_Omega * s_omega * c_inc;
    M1 = s_Omega * c_omega + c_Omega * s_omega * c_inc;
    N1 = s_omega * s_inc;
    L2 = -c_Omega * s_omega - s_Omega * c_omega * c_inc;
    M2 = -s_Omega * s_omega + c_Omega * c_omega * c_inc;
    N1 = c_omega * s_inc;
}

// Computes the (x, y, z) position of the orbit.
// Uses the equations defined in "Orbital Motions" by A. E. Roy 2005 pg. 93
void CPositionOrbit::Compute_xyz(double a, double beta, double e,
		double l1, double l2, double m1, double m2, double n1, double n2,
		double cos_E, double sin_E,
		float & x, float & y, float & z)
{
    x = float(a * (l1 * cos_E + beta * l2 * sin_E - e * l1));
    y = float(a * (m1 * cos_E + beta * m2 * sin_E - e * m1));
    z = float(a * (n1 * cos_E + beta * n2 * sin_E - e * n1));

    printf("x: %f y: %f z: %f \n", x, y, z);
}

void CPositionOrbit::GetXYZ(float & x, float & y, float & z)
{
	// Local variables (mostly renaming mParams variables for convenience).
	// Remember to convert the angular parameters into radians.
    double l1, l2, m1, m2, n1, n2;
    double Omega = (double) mParams[0] * PI / 180.0;
    double inc = (double) mParams[1] * PI / 180.0;
    double omega = (double) mParams[2] * PI / 180.0;
    double alpha = (double) mParams[3];
    double e = (double) mParams[4];
    double tau = (double) mParams[5];
    double T = (double) mParams[6];
    double t = (double) mTime;

	// Pre-compute a few values
    double n = ComputeN(T);
    double M = ComputeM(tau, n, t);
    double E = ComputeE(M, e);

    double cos_E = cos(E);
    double sin_E = sin(E);
    double beta = sqrt(1 - e*e);

    // Now compute the orbital coefficients
    Compute_Coefficients(Omega, inc, omega, l1, m1, n1, l2, m2, n2);
    Compute_xyz(alpha, beta, e, l1, l2, m1, m2, n1, n2, cos_E, sin_E, x, y, z);
}
