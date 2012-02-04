/*
 * misc.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  Miscellaneous, sometimes template, functions.
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

#endif /* MISC_H_ */
