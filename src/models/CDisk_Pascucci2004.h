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
 *  Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CDISK_PASCUCCI2004_H_
#define CDISK_PASCUCCI2004_H_

#include "CDensityDisk.h"

class CDisk_Pascucci2004: public CDensityDisk
{

public:
	CDisk_Pascucci2004();
	virtual ~CDisk_Pascucci2004();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "disk_pascucci2004"; };
};

#endif /* CDISK_PASCUCCI2004_H_ */
