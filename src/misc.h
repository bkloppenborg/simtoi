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

// Assigns input values to the output parameters using a boolean mask
template <class T>
void pull_params(T * in_params, int n_in_params, T * out_params, int n_out_params, bool * mask)
{
	int j = 0;
	for(int i = 0; i < n_out_params; i++)
	{
		if(mask[i])
		{
			// Assign the output parameter from the jth slot in the input buffer then increment input counter.
			out_params[i] = in_params[j];
			j++;
		}
	}

}


#endif /* MISC_H_ */
