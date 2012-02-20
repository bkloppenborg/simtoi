/*
 * CPositionOrbit.h
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 */

#ifndef CPOSITIONORBIT_H_
#define CPOSITIONORBIT_H_

// Constants for computing E.
double const ORBIT_THRESH = 1E-8;
int const ORBIT_MAX_ITERATIONS = 50;

#include "CPosition.h"

class CPositionOrbit: public CPosition
{
protected:
	float mTime;

public:
	CPositionOrbit();
	virtual ~CPositionOrbit();

protected:
	double ComputeN(double T);
	double ComputeM(double tau, double n, double t);
	double ComputeE(double M, double e);
	void Compute_Coefficients(double Omega, double inc, double omega,
			double & L1, double & M1, double & N1, double & L2, double & M2, double & N2);
	void Compute_xyz(double a, double beta, double e,
			double l1, double l2, double m1, double m2, double n1, double n2,
			double cos_E, double sin_E,
			double & x, double & y, double & z);

public:
	void GetXYZ(double & x, double & y, double & z);

	void SetTime(double t) { mTime = t; };
};

#endif /* CPOSITIONORBIT_H_ */
