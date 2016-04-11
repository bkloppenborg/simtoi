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
 * Copyright (c) 2015 Brian Kloppenborg
 */

#ifndef SRC_FACTORYBASE_H_
#define SRC_FACTORYBASE_H_

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>

using namespace std;

template <class ObjectPtr, class CreateFuncPtr>
class FactoryBase
{
protected:
	map<string, CreateFuncPtr> mCreators; ///< Associates id -> minimizer create function
	map<string, string> mNames; ///< Associates id -> minimizer create function

	FactoryBase() {};

public:
	/// \brief Create an instance of the specified object.
	///
	/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
	ObjectPtr create(const string & ID)
	{
		// try looking up the model by ID
		auto itCreators = mCreators.find(ID);
		if(itCreators != mCreators.end())
			return itCreators->second();

		// If this fails try looking up the ID from the name
		for(auto it: mNames)
		{
			if(it.second == ID)
				return create(it.first);
		}

		throw runtime_error("The item with ID '" + ID + "' does not exist within the factory.");

		return ObjectPtr();
	}

	/// \brief Creates an instance of the specified object from its name.
	ObjectPtr createFromName(const string & name)
	{
		string id = idFromName(name);
		return create(id);
	}

	/// \brief Returns a vector of the ids for the objects that are loaded
	vector<string> getIDs()
	{
		vector<string> temp;

		for(auto it: mCreators)
		{
			string id = it.first;
			temp.push_back(id);
		}

		return temp;
	}

	/// \brief Returns a vector containing the names of the objects that are loaded.
	vector<string> getNames()
	{
		vector<string> temp;

		for(auto it: mNames)
		{
			string name = it.second;
			temp.push_back(name);
		}

		return temp;
	}

	/// \brief Get the objects engine ID from the name of the engine
	string idFromName(const string & name)
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


	/// \brief Register a objects with the factory
	///
	/// \param CreateFunction A pointer to the object's constructor
	void addItem(CreateFuncPtr CreateFunction)
	{
		// Create a temporary instance of the minimizer to get information from it
		ObjectPtr object = CreateFunction();
		string ID = object->ID();
		string name = object->name();

		if(mCreators.find(ID) != mCreators.end())
			throw runtime_error("A minimizer with ID '" + ID + "' is already addItemed with CMinimizerFactory");

		// register the class
		mCreators[ID] = CreateFunction;
		mNames[ID] = name;
	}
};

#endif /* SRC_FACTORYBASE_H_ */
