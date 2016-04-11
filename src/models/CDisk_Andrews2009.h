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
 *  Copyright (c) 2013 Brian Kloppenborg
 */

#ifndef CDISK_ANDREWS2009_H_
#define CDISK_ANDREWS2009_H_

#include "CDensityDisk.h"

class CDisk_Andrews2009: public CDensityDisk
{

public:
	CDisk_Andrews2009();
	virtual ~CDisk_Andrews2009();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "disk_andrews2009"; };
};

#endif /* CDISK_ANDREWS2009_H_ */
