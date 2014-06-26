/*
 * CParameterMap.h
 *
 *  Created on: Jun 26, 2014
 *      Author: bkloppenborg
 */

#ifndef CPARAMETERMAP_H_
#define CPARAMETERMAP_H_

class CParameter;

#include <string>
#include <map>
using namespace std;

class CParameterMap
{
protected:
	map<string, CParameter> mParams; ///< The parameters used in this model.

public:
	CParameterMap();
	virtual ~CParameterMap();

	unsigned int addParameter(string internal_name, double value,
			double min, double max, bool free, double step_size,
			string human_name);
	unsigned int addParameter(string internal_name, double value,
			double min, double max, bool free, double step_size,
			string human_name, string help);

	const map<string, CParameter> & getParameterMap() { return mParams; };

	CParameter & getParameter(string id);
};

#endif /* CPARAMETERMAP_H_ */
