/*
 * CMinimizerFactory.cpp
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

#include "CMinimizerFactory.h"
#include <stdexcept>
#include <string>

#include "CModel.h"

using namespace std;

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins

#include "minimizers/CMinimizer_Benchmark.h"
#include "minimizers/CMinimizer_Bootstrap.h"
#include "minimizers/CMinimizer_GridSearch.h"
#include "minimizers/CMinimizer_levmar.h"
// Compiler directive to add support for MultiNest
#ifdef _ADD_MULTINEST
#include "minimizers/CMinimizer_MultiNest.h"
#endif // _ADD_MULTINEST

CMinimizerFactory::CMinimizerFactory()
{
	Register("benchmark", &CMinimizer_Benchmark::Create);
	Register("bootstrap_levmar", &CMinimizer_Bootstrap::Create);
	Register("gridsearch", &CMinimizer_GridSearch::Create);
	Register("levmar", &CMinimizer_levmar::Create);
#ifdef _ADD_MULTINEST
	Register("multinest", &CMinimizer_MultiNest::Create);
#endif // _ADD_MULTINEST
}

CMinimizerFactory::~CMinimizerFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified model.
/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
shared_ptr<CMinimizer> CMinimizerFactory::CreateMinimizer(string MinimizerID)
{
	auto it = mFactory.find(MinimizerID);
	if(it != mFactory.end())
		return it->second();

	throw runtime_error("The minimizer with ID '" + MinimizerID + "' not registered with CMinimizerFactory");

	return shared_ptr<CMinimizer>();
}

/// Returns a vector of the model names that are loaded.
vector<string> CMinimizerFactory::GetMinimizerList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the ModelFactory instance
CMinimizerFactory CMinimizerFactory::Instance()
{
	static CMinimizerFactory instance;
	return instance;
}

/// Registers a model with the name "ModelID" and creation function "CreateFunction" with the factory.
void CMinimizerFactory::Register(string MinimizerID, CreateMinimizerFn CreateFunction)
{
	if(mFactory.find(MinimizerID) != mFactory.end())
		throw runtime_error("A minimizer with ID '" + MinimizerID + "' is already registered with CMinimizerFactory");

	mFactory[MinimizerID] = CreateFunction;
}
