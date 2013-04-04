/*
 * \file CMinimizerFactory.cpp
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

#include "CMinimizerFactory.h"
#include <stdexcept>
#include <string>

#include "CModel.h"

using namespace std;

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins

#include "minimizers/CBenchmark.h"
#include "minimizers/CGridSearch.h"
#include "minimizers/CLevmar.h"
// Compiler directive to add support for MultiNest
#ifdef _ADD_MULTINEST
#include "minimizers/CMultiNest.h"
#endif // _ADD_MULTINEST
#include "minimizers/CBootstrap_Levmar.h"

/// \brief Private constructor. Use `Instance()` instead.
CMinimizerFactory::CMinimizerFactory()
{
	// TODO: For now we register minimizers explicitly. In the future, we should use plugins instead.
	Register("benchmark", &CBenchmark::Create);
	Register("bootstrap_levmar", &CBootstrap_Levmar::Create);
	Register("gridsearch", &CGridSearch::Create);
	Register("levmar", &CLevmar::Create);
#ifdef _ADD_MULTINEST
	Register("multinest", &CMultiNest::Create);
#endif // _ADD_MULTINEST
}

CMinimizerFactory::~CMinimizerFactory() \
{

}

/// \brief Create an instance of the specified model.
///
/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
shared_ptr<CMinimizerThread> CMinimizerFactory::CreateMinimizer(string MinimizerID)
{
	auto it = mFactory.find(MinimizerID);
	if(it != mFactory.end())
		return it->second();

	throw runtime_error("The minimizer with ID '" + MinimizerID + "' not registered with CMinimizerFactory");

	return shared_ptr<CMinimizerThread>();
}

/// \brief Returns a vector of the minimizer ids that are loaded.
vector<string> CMinimizerFactory::GetMinimizerList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// \brief Returns a copy of the CMinimizerFactory instance
CMinimizerFactory CMinimizerFactory::Instance()
{
	static CMinimizerFactory instance;
	return instance;
}

/// \brief Registers a minimizer with the factory
///
/// \param MinimizerID A unique string which identifies this minimizer.
/// \param CreateFunction A pointer to a CMinimizer::Create
void CMinimizerFactory::Register(string MinimizerID, CreateMinimizerFn CreateFunction)
{
	if(mFactory.find(MinimizerID) != mFactory.end())
		throw runtime_error("A minimizer with ID '" + MinimizerID + "' is already registered with CMinimizerFactory");

	mFactory[MinimizerID] = CreateFunction;
}
