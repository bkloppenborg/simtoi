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
 *  Copyright (c) 2014 Brian Kloppenborg
 */

#ifndef CUNIFORMSPOT_H_
#define CUNIFORMSPOT_H_

#include "CFeature.h"

class CUniformSpot: public CFeature
{

public:
	CUniformSpot();
	virtual ~CUniformSpot();

	static shared_ptr<CFeature> Create()
	{
		return shared_ptr<CFeature>(new CUniformSpot());
	}

	void apply(CModel * model);
};

#endif /* CUNIFORMSPOT_H_ */
