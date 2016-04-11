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
 * Copyright (c) 2014 Brian Kloppenborg
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
	string id;	/// An internal name for the parameter, used in seralization
	string human_name;		/// A human-friendly name used in GUI elements
	string help_text;		/// Help text displayed to the user in the GUI.

	bool check_bounds;		/// Whether or not this object should check that values are within bounds.

public:
	CParameter();
	virtual ~CParameter();

	void	clearFlags();

	double 	getValue(bool normalized = false) const;
	double 	getMin() const { return min; };
	double 	getMax() const { return max; };
	double 	getStepSize() const { return step_size; };
	double 	getDecimalPlaces() const { return decimal_places; };
	string 	getID() const { return id; };
	string	getHumanName() const { return human_name; };
	string  getHelpText() const { return help_text; };

	bool   	isDirty() const { return dirty; };
	bool   	isFree() const { return free; };

	void	setDecimalPlaces(unsigned int num_places);
	void	setValue(double new_value, bool is_normalized = false);
	void	setMin(double new_min);
	void	setMax(double new_max);
	void	setStepSize(double new_step_size);
	void	setDirty(bool is_dirty);
	void	setFree(bool is_free);
	void	setID(string new_id);
	void	setHumanName(string new_human_name);
	void	setHelpText(string new_help_text);

	void 	toggleBoundsChecks(bool enable_checks);
};

#endif /* CPARAMETER_H_ */
