/*
 * CPositionFactory.h
 *
 *  Created on: Feb 10, 2013
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

#ifndef CPOSITIONFACTORY_H_
#define CPOSITIONFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CPosition;

typedef shared_ptr<CPosition> (*CreatePositionFn)(void);


class CPositionFactory {
private:
//	static CPositionFactory * mInstance;

	map<string, CreatePositionFn> mFactory;

	CPositionFactory();

public:
	virtual ~CPositionFactory();

public:
	shared_ptr<CPosition> CreatePosition(string PositionID);

	static CPositionFactory Instance();

	void Register(string PositionID, CreatePositionFn CreateFunction);

	vector<string> GetPositionList();
};

#endif /* CPOSITIONFACTORY_H_ */
