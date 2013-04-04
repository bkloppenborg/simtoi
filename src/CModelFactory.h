/*
 * \file CModelFactory.h
 *
 *  Created on: Feb 10, 2013
 *  \author bkloppenborg
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

#ifndef CMODELFACTORY_H_
#define CMODELFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CModel;
typedef shared_ptr<CModel> CModelPtr;

typedef shared_ptr<CModel> (*CreateModelFn)(void);

/// \brief A factory class for creating/registering SIMTOI models
///
/// This factory is used to register and create instances of models in SIMTOI.
/// As this class is a singleton, the constructor is private. Call the `Instance()`
/// function to get a copy of this object.
///
/// Before any model may be used in SIMTOI, it must be registered with this class.
/// Simply call `Register()` with a unique ID and `CModelPtr` creation function.
/// After the model has been registered it will become available for use in SIMTOI.
class CModelFactory {
private:
	map<string, CreateModelFn> mFactory;

	CModelFactory();

public:
	virtual ~CModelFactory();

public:
	shared_ptr<CModel> CreateModel(string ModelID);

	static CModelFactory Instance();

	void Register(string ModelID, CreateModelFn CreateFunction);

	vector<string> GetModelList();
};

#endif /* CMODELFACTORY_H_ */
