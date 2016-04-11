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

#ifndef CPOSITIONFACTORY_H_
#define CPOSITIONFACTORY_H_

#include "FactoryBase.h"

class CPosition;
typedef shared_ptr<CPosition> CPositionPtr;
typedef shared_ptr<CPosition> (*CreatePositionFn)(void);

class CPositionFactory : public FactoryBase<CPositionPtr, CreatePositionFn> {

private:
	CPositionFactory() {};
	CPositionFactory(CPositionFactory const&) = delete;
	void operator=(CPositionFactory const&)    = delete;

public:
	/// \brief Returns a copy of the CMinimizerFactory instance
	static CPositionFactory & getInstance()
	{
		static CPositionFactory instance;
		return instance;
	}
};

#endif /* CPOSITIONFACTORY_H_ */
