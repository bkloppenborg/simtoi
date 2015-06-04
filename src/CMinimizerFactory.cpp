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

using namespace std;

#include "QT/CMinimizerThread.h"

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins

#include "minimizers/CBenchmark.h"
#include "minimizers/CGridSearch.h"
// Compiler directive to add support for levmar
#ifdef _ADD_LEVMAR
#include "minimizers/CLevmar.h"
#include "minimizers/CBootstrap_Levmar.h"
#endif //_ADD_LEVMAR
// Compiler directive to add support for MultiNest
#ifdef _ADD_MULTINEST
#include "minimizers/CMultiNest.h"
#endif // _ADD_MULTINEST
#ifdef _ADD_NLOPT
#include "minimizers/CNLopt.h"
#endif // _ADD_NLOPT

/// \brief Private constructor. Use `Instance()` instead.
CMinimizerFactory::CMinimizerFactory()
{
	// TODO: For now we addItem minimizers explicitly. In the future, we should use plugins instead.
  addItem(&CBenchmark::Create);
  addItem(&CGridSearch::Create);

#ifdef _ADD_NLOPT
  addItem(&CNLopt::CreateNELDERMEAD);
  addItem(&CNLopt::CreateDIRECTL);
  addItem(&CNLopt::CreateDIRECT);
  addItem(&CNLopt::CreateCRS2);
  addItem(&CNLopt::CreateMLSLLDS);
//  addItem("&CNLopt::CreateSTOGORAND); // missing from Ubuntu's NLOpt library, disable
  addItem(&CNLopt::CreateISRES);
  addItem(&CNLopt::CreateESCH);
  addItem(&CNLopt::CreateCOBYLA);
  addItem(&CNLopt::CreateBOBYQA);
  addItem(&CNLopt::CreateNEWUOA);
  addItem(&CNLopt::CreatePRAXIS);
  addItem(&CNLopt::CreateSBPLX);
#endif //_ADD_NLOPT

#ifdef _ADD_LEVMAR
  addItem(&CLevmar::Create);
  addItem(&CBootstrap_Levmar::Create);
#endif //_ADD_LEVMAR

#ifdef _ADD_MULTINEST
  addItem(&CMultiNest::Create);
#endif // _ADD_MULTINEST


}

CMinimizerFactory::~CMinimizerFactory() \
{

}

/// \brief Create an instance of the specified model.
///
/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
shared_ptr<CMinimizerThread> CMinimizerFactory::create(string ID)
{
	auto it = mCreators.find(ID);
	if(it != mCreators.end())
		return it->second();

	throw runtime_error("The item with ID '" + ID + "' does not exist within the factory.");

	return shared_ptr<CMinimizerThread>();
}

/// \brief Returns a vector of the ids for the minimizer that are loaded
vector<string> CMinimizerFactory::getIDs()
{
	vector<string> temp;

	for(auto it: mCreators)
	{
		string id = it.first;
		temp.push_back(id);
	}

	return temp;
}

/// \brief Returns a vector containing the names of the minimizers that are loaded.
vector<string> CMinimizerFactory::getNames()
{
	vector<string> temp;

	for(auto it: mNames)
	{
		string name = it.second;
		temp.push_back(name);
	}

	return temp;
}

string CMinimizerFactory::idFromName(string name)
{
	string id = "";

	for(auto it: mNames)
	{
		if(it.second == name)
			id = it.first;
	}

	if(id.length() < 1)
		throw runtime_error("A minimizer with name '" + name + "' is already addItemed with CMinimizerFactory");

	return id;
}

/// \brief Returns a copy of the CMinimizerFactory instance
CMinimizerFactory CMinimizerFactory::getInstance()
{
	static CMinimizerFactory instance;
	return instance;
}

/// \brief Register a minimizer with the factory
///
/// \param MinimizerID A unique string with no spaces that identifies this minimizer.
///        This parameter may be used on the command line so keep it short.
/// \param CreateFunction A pointer to a CMinimizer::Create
void CMinimizerFactory::addItem(CreateMinimizerFn CreateFunction)
{
	// Create a temporary instance of the minimizer to get information from it
	CMinimizerPtr minimizer = CreateFunction();
	string ID = minimizer->ID();
	string name = minimizer->name();

	if(mCreators.find(ID) != mCreators.end())
		throw runtime_error("A minimizer with ID '" + ID + "' is already addItemed with CMinimizerFactory");

	// register the class
	mCreators[ID] = CreateFunction;
	mNames[ID] = name;
}
