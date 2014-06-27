/*
 * CParameterList.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: bkloppenborg
 */

#include "CParameterMap.h"
#include "CParameter.h"

CParameterMap::CParameterMap()
{
	// TODO Auto-generated constructor stub

}

CParameterMap::~CParameterMap()
{
	// TODO Auto-generated destructor stub
}

/// Adds an additional parameter for this model with no help text
unsigned int CParameterMap::addParameter(string internal_name, double value, double min, double max, bool free, double step_size,
		string human_name)
{
	return addParameter(internal_name, value, min, max, free, step_size, human_name, string());
}

/// Adds an additional parameter for this model
unsigned int CParameterMap::addParameter(string internal_name, double value, double min, double max, bool free, double step_size,
		string human_name, string help)
{
	// create the parameter, set some default values.
	CParameter temp;
	temp.setID(internal_name);
	temp.setMin(min);
	temp.setMax(max);
	temp.setValue(value);
	temp.setFree(free);
	temp.setStepSize(step_size);
	temp.setHumanName(human_name);
	temp.setHelpText(help);

	// append it to the vector
	mParams[internal_name] = temp;

	// return the parameter number
	return mParams.size() - 1;
}

/// Clears all flags set on the parameters
void CParameterMap::clearFlags()
{
	for(map<string,CParameter>::iterator it = mParams.begin(); it != mParams.end(); ++it)
		it->second.clearFlags();
}

/// Copies the nominal values of the free parameters into the `params` array
///
/// @param params A double array of size 'n_params' to which the parameters will be copied
/// @param n_params The size of `params`
/// @param normalize_value Whether or not the parameters should be normalized.
unsigned int CParameterMap::getFreeParameters(double * params, unsigned int n_params, bool normalize_value)
{
	unsigned int n = 0;
	for(auto it: mParams)
	{
		if(n > n_params)
			break;

		if(it.second.isFree())
		{
			params[n] = it.second.getValue(normalize_value);
			n++;
		}
	}

	return n;
}

/// Returns a vector full of pairs of min (first) max (second) values for
/// the free parameters.
vector<pair<double,double> > CParameterMap::getFreeParameterMinMaxes()
{
	vector< pair<double, double> > min_maxes;

	for(auto it: mParams)
	{
		if(it.second.isFree())
		{
			pair<double, double> tmp;
			tmp.first = it.second.getMin();
			tmp.second = it.second.getMax();
			min_maxes.push_back(tmp);
		}
	}

	return min_maxes;
}

/// Returns the step sizes of the free parameters.
unsigned int CParameterMap::getFreeParameterStepSizes(double * steps, unsigned int size)
{
	int n = 0;

	for(auto it: mParams)
	{
		if(n > size)
			break;

		if(it.second.isFree())
		{
			steps[n] = it.second.getStepSize();
			n++;
		}
	}

	return n;
}

/// Counts the number of free parameters in the map.
unsigned int CParameterMap::getFreeParameterCount()
{
	unsigned int n = 0;
	for(auto it: mParams)
		if(it.second.isFree()) n++;

	return n;
}

/// \brief Returns a vector of strings containing the names of the free parameters
/// prefixed with the name of the parent object.
vector<string> CParameterMap::getFreeParameterNames()
{
	vector<string> tmp;
	for(auto it: mParams)
	{
		if(it.second.isFree())
			tmp.push_back(name + '.' + it.second.getHumanName());
	}

	return tmp;
}

/// Returns a reference to the specified parameter or throws an out_of_range
/// exception if the key does not exist.
CParameter & CParameterMap::getParameter(string id)
{
	return mParams.at(id);
}

/// \brief Restores parameters values from the JSON value
///
/// Restores parameter values, names, and min/max values from a JSON save file.
/// Parameters are imported by the IDs specified in the mParams map. If the
/// parameter is not found, the default values provided in the constructor
/// are left intact.
void CParameterMap::restore(Json::Value input)
{
	// Only restore parameters that we expect will exist.
	for(map<string,CParameter>::iterator it = mParams.begin(); it != mParams.end(); ++it)
	{
		CParameter & param = it->second;
		string id = param.getID();

		// Disable bounds checking while restoring values.
		// Hopefully the save files are consistent!
		param.toggleBoundsChecks(false);

		param.setValue(    double( input[id][0u].asDouble() ));
		param.setMin(      double( input[id][1u].asDouble() ));
		param.setMax(      double( input[id][2u].asDouble() ));
		param.setFree(     bool  ( input[id][3u].asBool()   ));
		param.setStepSize( double( input[id][4u].asDouble() ));

		// Enable bounds checking after values are restored.
		param.toggleBoundsChecks(true);
	}
}

/// \brief Serializes the parameters to a Json::Value object
///
/// Converts the map into a Json::value object in the following format:
/// 	`name : [value, min, max, free, step_size]`.
///
/// NOTE: This method can be overridden; however, it is suggested that this function
/// be called by any overriding classes to ensure proper object serialization.
/// For example, see `CModel::Serialize`.
Json::Value CParameterMap::serialize()
{
	Json::Value output;

	for(auto it: mParams)
	{
		CParameter & param = it.second;
		Json::Value tmp;

		tmp.append(Json::Value(param.getValue()));
		tmp.append(Json::Value(param.getMin()));
		tmp.append(Json::Value(param.getMax()));
		tmp.append(Json::Value(param.isFree()));
		tmp.append(Json::Value(param.getStepSize()));
		output[param.getID()] = tmp;
	}

	return output;
}

/// Sets the free parameter values from an input array of doubles. Returns the
/// number of values set during the call.
///
/// @param values An input array of parameter values
/// @param n_values The number of elements in values
/// @param normalized_values Whether or not the values are normalized
unsigned int CParameterMap::setFreeParameterValues(double * values, unsigned int n_values, bool normalized_values)
{
	int n = 0;
	for(auto & it: mParams)
	{
		if(n > n_values)
			break;

		if(it.second.isFree())
		{
			it.second.setValue(values[n], normalized_values);
			n++;
		}
	}

	return n;
}
