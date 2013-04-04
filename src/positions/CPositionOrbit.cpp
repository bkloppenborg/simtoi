/*
 * CPositionOrbit.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
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
	mTime = 0;
	mName = "Orbit";
	mPositionID = "orbit_bound";

	mParamNames.push_back("Inc");	// Inclination, keep as element 0 for CModel::SetAnglesFromPosition
	mParamNames.push_back("Omega");	// Omega, keep as elements 1 for CModel::SetAnglesFromPosition
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

    L1 = c_Omega * c_omega - s_Omega * s_omega * c_inc;
    M1 = s_Omega * c_omega + c_Omega * s_omega * c_inc;
    N1 = s_omega * s_inc;
    L2 = -c_Omega * s_omega - s_Omega * c_omega * c_inc;
    M2 = -s_Omega * s_omega + c_Omega * c_omega * c_inc;
    N2 = c_omega * s_inc;
}

// Computes the (x, y, z) position of the orbit.
// Uses the equations defined in "Orbital Motions" by A. E. Roy 2005 pg. 93
void CPositionOrbit::Compute_xyz(double a, double beta, double e,
		double l1, double l2, double m1, double m2, double n1, double n2,
		double cos_E, double sin_E,
		double & x, double & y, double & z)
{
    x = a * (l1 * cos_E + beta * l2 * sin_E - e * l1);
    y = a * (m1 * cos_E + beta * m2 * sin_E - e * m1);
    z = a * (n1 * cos_E + beta * n2 * sin_E - e * n1);
}

CPositionPtr CPositionOrbit::Create()
{
	return CPositionPtr(new CPositionOrbit());
}

void CPositionOrbit::GetXYZ(double & x, double & y, double & z)
{
	// Local variables (mostly renaming mParams variables for convenience).
	// Remember to convert the angular parameters into radians.
    double l1, l2, m1, m2, n1, n2;
    double inc = mParams[0] * PI / 180.0;
    double Omega = mParams[1] * PI / 180.0;
    double omega = mParams[2] * PI / 180.0;
    double alpha = mParams[3];
    double e = mParams[4];
    double tau = mParams[5];
    double T = mParams[6];
    double t = mTime;

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
