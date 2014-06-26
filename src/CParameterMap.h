/*
 * CParameterMap.h
 *
 *  Created on: Jun 26, 2014
 *      Author: bkloppenborg
 */

#ifndef CPARAMETERMAP_H_
#define CPARAMETERMAP_H_

#include <string>
#include <map>
using namespace std;

class CParameter;
#include "json/json.h"

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

	void clearFlags();

	void getFreeParameters(double * params, unsigned int n_params, bool normalize_value);
	unsigned int getFreeParameterCount();
	const map<string, CParameter> & getParameterMap() { return mParams; };

	CParameter & getParameter(string id);

	void restore(Json::Value input);
	Json::Value serialize();
};

#endif /* CPARAMETERMAP_H_ */
