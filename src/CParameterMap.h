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
	string mName;					///< A human-readable name for the object. Try to limit to < 60 characters
	string mID;						///< An internal ID for this object

public:
	CParameterMap();
	virtual ~CParameterMap();

	unsigned int addParameter(string internal_name, double value,
			double min, double max, bool free, double step_size,
			string human_name, unsigned int decimal_places=1);
	unsigned int addParameter(string internal_name, double value,
			double min, double max, bool free, double step_size,
			string human_name, string help, unsigned int decimal_places=1);

	void clearFlags();

	unsigned int getAllParameters(double * params, unsigned int n_params, bool normalize_value = false);
	unsigned int getFreeParameters(double * params, unsigned int n_params, bool normalize_value = false);
	unsigned int getFreeParameterCount();
	vector<string> getFreeParameterNames();
	vector<pair<double,double> > getFreeParameterMinMaxes();
	unsigned int getFreeParameterStepSizes(double * steps, unsigned int size);

	const map<string, CParameter> & getParameterMap() { return mParams; };
	virtual string ID() const { return mID; };
	virtual string name() const { return mName; };
	CParameter & getParameter(string id);

	virtual bool isDirty();

	void restore(Json::Value input);

	Json::Value serialize();

	virtual unsigned int setFreeParameterValues(double * values, unsigned int n_values, bool normalized_values = false);
	void setParameter(const string & name, double value, bool is_normalized = false);
};

#endif /* CPARAMETERMAP_H_ */
