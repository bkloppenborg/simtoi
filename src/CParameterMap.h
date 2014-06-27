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

#include "json/json.h"

#include "CParameter.h"

class CParameterMap
{
protected:
	map<string, CParameter> mParams; ///< The parameters used in this model.
	string name;						///< A human-readable name for the object
	string id;						///< An internal ID for this object

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

	unsigned int getFreeParameters(double * params, unsigned int n_params, bool normalize_value = false);
	unsigned int getFreeParameterCount();
	vector<string> getFreeParameterNames();
	vector<pair<double,double> > getFreeParameterMinMaxes();
	unsigned int getFreeParameterStepSizes(double * steps, unsigned int size);

	const map<string, CParameter> & getParameterMap() { return mParams; };
	virtual string getID() const { return id; };
	virtual string getName() const { return name; };
	CParameter & getParameter(string id);

	void restore(Json::Value input);

	Json::Value serialize();

	unsigned int setFreeParameterValues(double * values, unsigned int n_values, bool normalized_values = false);
};

#endif /* CPARAMETERMAP_H_ */
