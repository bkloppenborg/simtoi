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
 * Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CPOSITIONORBIT_H_
#define CPOSITIONORBIT_H_

// Constants for computing E.
double const ORBIT_THRESH = 1E-8;
int const ORBIT_MAX_ITERATIONS = 50;

#include "CPosition.h"

class CPositionOrbit: public CPosition
{
	friend class CBinaryOrbit;

public:
	CPositionOrbit();
	virtual ~CPositionOrbit();

protected:
	double ComputeN(double P);
	double ComputeM(double T, double n, double t);
	double ComputeE(double M, double e);
	void Compute_Coefficients(double Omega, double inc, double omega,
			double & L1, double & M1, double & N1, double & L2, double & M2, double & N2);
	void Compute_xyz(double a, double beta, double e,
			double l1, double m1, double n1, double l2, double m2, double n2,
			double cos_E, double sin_E,
			double & x, double & y, double & z);

public:
	static CPositionPtr Create();

	void GetAngles(double & Omega_t, double & inc_t, double & omega_t);
	void GetXYZ(double & x, double & y, double & z);
};

#endif /* CPOSITIONORBIT_H_ */
