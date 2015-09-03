/*
 * CLinearMotion.h
 *
 *  Created on: Sep 1 2015
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

#include "CLinearMotion.h"

CLinearMotion::CLinearMotion()
{
	mName = "Linear Motion";
	mID = "linear";
	mPositionType = STATIC;

	addParameter("t0", 0, 0, 3E6, false, 0.1, "t0", "Time for the zero point", 2);

	addParameter("N", 0, -1, 1, false, 0.1, "N_0", "North (mas) (positive is up on the screen)", 2);
	addParameter("vN", 0, -1, 1, false, 0.01, "v_N", "Velocity in the North direction (mas/day)", 3);
	addParameter("aN", 0, -1, 1, false, 0.01, "a_N", "Acceleration in the North direction (mas/(day^2))", 4);

	addParameter("E", 0, -1, 1, false, 0.1, "E_0", "East (mas) (positive is left on the screen)", 2);
	addParameter("vE", 0, -1, 1, false, 0.01, "v_E", "Velocity in the East direction (mas/day)", 3);
	addParameter("aE", 0, -1, 1, false, 0.01, "a_E", "Acceleration in the East direction (mas/(day^2))", 4);

	addParameter("Z", 0, -1, 1, false, 0.1, "Z_0", "Z-direction (mas) (positive is into of the screen)", 2);
	addParameter("vZ", 0, -1, 1, false, 0.01, "v_Z", "Velocity in the Z direction (mas/day)", 3);
	addParameter("aZ", 0, -1, 1, false, 0.01, "a_Z", "Acceleration in the Z direction (mas/(day^2))", 4);
}

CLinearMotion::~CLinearMotion()
{
	// Do nothing
}

CPositionPtr CLinearMotion::Create()
{
	return CPositionPtr(new CLinearMotion());
}

void CLinearMotion::GetXYZ(double & x, double & y, double & z)
{
	// Compute the (N,E,Z) positions based upon simple linear motion.
    double t = mTime - mParams["t0"].getValue();
    double t2 = t * t;

    // North
    double N0 = mParams["N"].getValue();
    double vN = mParams["vN"].getValue();
    double aN = mParams["aN"].getValue();
    double astro_north = N0 + vN*t + 0.5 * aN * t2;
    // East
    double E0 = mParams["E"].getValue();
    double vE = mParams["vE"].getValue();
    double aE = mParams["aE"].getValue();
    double astro_east = E0 + vE*t + 0.5 * aE * t2;
    // Z
    double Z0 = mParams["Z"].getValue();
    double vZ = mParams["vZ"].getValue();
    double aZ = mParams["aZ"].getValue();
    double astro_z = Z0 + vZ*t + 0.5 * aZ * t2;

	// Astronomical convention has (North,East,z) = (Up,Left,away),
	// OpenGL has (x,y,z) = (right,up,towards)
	x = astro_east;
	y = astro_north;
	z = astro_z;
}
