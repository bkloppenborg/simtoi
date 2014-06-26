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
	temp.setInternalName(internal_name);
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

/// Returns a reference to the specified parameter or throws an out_of_range
/// exception if the key does not exist.
CParameter & CParameterMap::getParameter(string id)
{
	return mParams.at(id);
}
