/*
 * misc.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  Miscellaneous, sometimes template, functions.
 */

/* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
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
 */

#ifndef MISC_H_
#define MISC_H_

template <class T>
/// Assigns input values to the output parameters using a boolean mask
void push_params(T * in_params, int n_in_params, T * out_params, int n_out_params, bool * mask)
{
	int j = 0;
	for(int i = 0; (i < n_out_params); i++)
	{
		if(mask[i])
		{
			// Assign the output parameter from the jth slot in the input buffer then increment input counter.
			out_params[i] = in_params[j];
			j++;
		}
	}
}

template <class T>
/// Copies values from in_params to out_params using a boolean mask, respecting the sizes of both arrays.
void pull_params(T * in_params, int n_in_params, T * out_params, int n_out_params, bool * mask)
{
	int j = 0;
	for(int i = 0; (i < n_in_params && j < n_out_params); i++)
	{
		if(mask[i])
		{
			// Assign the output parameter from the jth slot in the input buffer then increment input counter.
			out_params[j] = in_params[i];
			j++;
		}
	}
}

template <typename T>
double sum(T * input, int n_input)
{
	double tmp = 0;
	for(unsigned int i = 0; i < n_input; i++)
		tmp += input[i];

	return tmp;
}

template <typename T>
void square(T * input, int n_input)
{
	for(unsigned int i = 0; i < n_input; i++)
		input[i] = input[i] * input[i];
}

#endif /* MISC_H_ */
