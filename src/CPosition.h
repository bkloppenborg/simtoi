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

#ifndef CPOSITION_H_
#define CPOSITION_H_

#include <vector>
#include <utility>
#include <memory>
using namespace std;

#include "CParameterMap.h"

class CPosition;
typedef shared_ptr<CPosition> CPositionPtr;

class CPosition : public CParameterMap
{
public:
	enum PositionTypes
	{
		STATIC,	// not time dependent
		ORBIT,	// IS time dependent and contains orbital elements (Omega, inclination, and omega)
//		DYNAMIC	// IS time dependent, but does NOT contain orbital elements
	};

protected:
	PositionTypes mPositionType;
	double mTime;

public:
	CPosition();
	virtual ~CPosition();

	string ID() { return mID; }
	string name() { return mName; }

	PositionTypes GetPositionType() { return mPositionType; };

	// Computes the (X,Y,Z) position of an object.  Z should be set to zero if not computed.
	virtual void GetXYZ(double & x, double & y, double & z);
	virtual void GetAngles(double & Omega_t, double & inc_t, double & omega_t);

	virtual void SetTime(double time);
};

#endif /* CPOSITION_H_ */
