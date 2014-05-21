/*
 * CFeatureFactory.h
 *
 *  Created on: Apr. 23, 2014
 *      Author: bkloppenborg
 */

 /*
 * Copyright (c) 2014 Brian Kloppenborg
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

#ifndef CFEATUREFACTORY_H_
#define CFEATUREFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CFeature;

typedef shared_ptr<CFeature> (*CreateFeatureFn)(void);


class CFeatureFactory {
private:

	map<string, CreateFeatureFn> mFactory;

	CFeatureFactory();

public:
	virtual ~CFeatureFactory();

public:
	shared_ptr<CFeature> CreateFeature(string PositionID);

	static CFeatureFactory Instance();

	void Register(string FeatureID, CreateFeatureFn CreateFunction);

	vector<string> GetFeatureList();
};

#endif /* CFEATUREFACTORY_H_ */
