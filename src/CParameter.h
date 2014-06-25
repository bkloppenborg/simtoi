/*
 * CParameter.h
 *
 *  Created on: Jun 25, 2014
 *      Author: bkloppenborg
 */

#ifndef CPARAMETER_H_
#define CPARAMETER_H_

#include <string>
using namespace std;

class CParameter
{
private:
	double value;			/// The nominal value of the parameter, stored in native units
	double min;				/// The minimum allowable value of the parameter, stored in native units
	double max;				/// The maximum allowable value of the parameter, stored in native units
	double step_size;		/// The step size. (Used with various minimization engines.)
	bool dirty;				/// Whether or not the parameter has recently been updated.
	bool free;				/// Whether or not the parameter is free (or fixed) for the minimization process.
	unsigned int decimal_places;	/// The number of decimal places to display in the GUI or serialize to a file.
	double precision;
	string internal_name;	/// An internal name for the parameter, used in seralization
	string human_name;		/// A human-friendly name used in GUI elements
	string help_text;		/// Help text displayed to the user in the GUI.

public:
	CParameter();
	virtual ~CParameter();

	double 	getValue(bool normalized = false) const;
	double 	getMin() const { return min; };
	double 	getMax() const { return max; };
	double 	getStepSize() const { return step_size; };
	double 	getDecimalPlaces() const { return decimal_places; };
	bool   	isDirty() const { return dirty; };
	bool   	isFree() const { return free; };
	string 	getInternalName() const { return internal_name; };
	string	getHumanName() const { return human_name; };
	string  getHelpText() const { return help_text; };

	void	setDecimalPlaces(unsigned int num_places);
	void	setValue(double new_value, bool is_normalized = false);
	void	setMin(double new_min);
	void	setMax(double new_max);
	void	setStepSize(double new_step_size);
	void	setDirty(bool is_dirty);
	void	setFree(bool is_free);
	void	setInternalName(string new_internal_name);
	void	setHumanName(string new_human_name);
	void	setHelpText(string new_help_text);
};

#endif /* CPARAMETER_H_ */
