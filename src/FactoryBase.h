/*
 * FactoryBase.h
 *
 *  Created on: Jun 5, 2015
 *      Author: bkloppenborg
 *
 *  A template base class for factory classes.
 */

#ifndef SRC_FACTORYBASE_H_
#define SRC_FACTORYBASE_H_

#include <string>
#include <map>
#include <memory>
#include <vector>

using namespace std;

template <class ObjectPtr, class CreateFuncPtr>
class FactoryBase
{
protected:
	map<string, CreateFuncPtr> mCreators; ///< Associates id -> minimizer create function
	map<string, string> mNames; ///< Associates id -> minimizer create function

	FactoryBase() {};

public:
	/// \brief Create an instance of the specified model.
	///
	/// Returns a shared_ptr<CModel> to the object if found, or throws a runtime exception.
	ObjectPtr create(string ID)
	{
		auto it = mCreators.find(ID);
		if(it != mCreators.end())
			return it->second();

		throw runtime_error("The item with ID '" + ID + "' does not exist within the factory.");

		return ObjectPtr();
	}

	/// \brief Returns a vector of the ids for the minimizer that are loaded
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

	/// \brief Returns a vector containing the names of the minimizers that are loaded.
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

	/// \brief Get the minimization engine ID from the name of the engine
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


	/// \brief Register a minimizer with the factory
	///
	/// \param MinimizerID A unique string with no spaces that identifies this minimizer.
	///        This parameter may be used on the command line so keep it short.
	/// \param CreateFunction A pointer to a CMinimizer::Create
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
