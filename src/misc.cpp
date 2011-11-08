/*
 * misc.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

// Assigns output[i] = input[j] when mask is true.
void pull_params(double * in_params, int n_in_params, double * out_params, int n_out_params, bool * mask)
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
