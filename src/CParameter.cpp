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

#include "CParameter.h"

#include <stdexcept>
#include <cmath>
#include <limits>
using namespace std;

CParameter::CParameter()
{
	// Put in some (reasonable) default values.
	value = 0;
	min = numeric_limits<double>::lowest();
	max = numeric_limits<double>::max();
	step_size = 0.1;
	dirty = true;
	free = false;
	decimal_places = 1;
	precision = 1E-10;
	id = "invalid_id";
	human_name = "Text not set by developer.";
	help_text = "Text not set by developer.";
	check_bounds = true;
}

CParameter::~CParameter()
{
	// TODO Auto-generated destructor stub
}

/// Clears flags that may have been set due to parameters changing value.
void 	CParameter::clearFlags()
{
	dirty = false;
}

/// Returns the value of the parameter.
///
/// @param normalized 	If set to true, returns the value in the range [0, 1],
/// 					otherwise the value is returned in its native units.
double 	CParameter::getValue(bool normalized) const
{
	// If we need to normalize the value, follow this equation:
	// f(x) = (x - min) / (max - min) // the normalized value
	// x = f(x) * (max - min) + min   // restore the original value.
	if(normalized)
		return (value - min) / (max - min);

	return value;
}

#include <iostream>

/// Sets the number of decimal places / precision for this parameter
///
/// @param n_decimal_places The minimum number of decimal places for this parameter
void CParameter::setDecimalPlaces(unsigned int n_decimal_places)
{
	decimal_places = n_decimal_places;
	precision = 1*pow(10.0, -1.0 * decimal_places);
}

/// Sets the value
void CParameter::setValue(double new_value, bool is_normalized)
{
	// Scale the parameters into regular units
	// f(x) = (x - min) / (max - min) // the normalized value
	// x = f(x) * (max - min) + min   // restore the original value.
	if(is_normalized)
		new_value = new_value * (max - min) + min;

	// Check that value is within bounds
	if(check_bounds && new_value > max)
		throw range_error("The new value for " + human_name + " exceeds the set maximum.");
	else if(check_bounds && new_value < min)
		throw range_error("The new value for " + human_name + " is less than the set minimum.");

	// set the dirty flag, if needed.
	if(fabs(value - new_value) > precision)
		dirty = true;
	else
		dirty = false;

	value = new_value;
}

/// Sets the parameter's minimum value.
///
/// An range_error exception is thrown if min < max.
void CParameter::setMin(double new_min)
{
	if(check_bounds && new_min > max)
		throw range_error("New minimum value is less than the specified maximum.");

	min = new_min;
}

/// Sets the parameter's maximum value.
///
/// An range_error exception is thrown if max < min.
void CParameter::setMax(double new_max)
{
	if(check_bounds && new_max < min)
		throw range_error("New maximum value is less than the specified minimum.");

	max = new_max;
}

/// Sets the step size.
///
/// @param new_step_size The new step size. This value must be greater than zero.
void CParameter::setStepSize(double new_step_size)
{
	if(check_bounds && !(new_step_size > 0))
		throw range_error("The step size cannot be less than 0.");

	step_size = new_step_size;
}

/// Sets whether or not this parameter is dirty (i.e. the value has changed)
///
/// Normally the setValue() function will set the dirty flag automatically if
/// the value changes by more than the specified precision. In some
/// circumstances it may be desirable to change the state of the dirty flag,
/// hence this function.
void CParameter::setDirty(bool is_dirty)
{
	dirty = is_dirty;
}

/// Sets whether or not this parameter is free for minimization.
void CParameter::setFree(bool is_free)
{
	free = is_free;
}

/// Sets the internal name for this parameter.
///
/// After the objects is initalized, this probably shouldn't be changed.
void CParameter::setID(string new_id)
{
	id = new_id;
}

/// Sets a new human-friendly name for this parameter.
void CParameter::setHumanName(string new_human_name)
{
	human_name = new_human_name;
}

/// Sets the help text for this parameter.
void CParameter::setHelpText(string new_help_text)
{
	help_text = new_help_text;
}

/// Enable or disable value/min/max bounds checking
void CParameter::toggleBoundsChecks(bool enable_checks)
{
	check_bounds = enable_checks;
}
