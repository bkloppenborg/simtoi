/*
 * CPosition.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A class serving as a base-class and lookup class for positions.
 *  If you add a new position type, create
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
		STATIC,
		DYNAMIC
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

	virtual void SetTime(double time);
};

#endif /* CPOSITION_H_ */
