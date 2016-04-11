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
 * Copyright (c) 2011 Brian Kloppenborg
 */

#ifndef MISC_H_
#define MISC_H_

#include <limits>

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

// Floating-point modulo
// The result (the remainder) has the same sign as the divisor.
// Similar to matlab's mod(); Not similar to fmod() -   Mod(-3,4)= 1   fmod(-3,4)= -3
// The original article from which this function was taken is distributed under
// The Code Project Open License (CPOL) 1.02, a fully permissive license.
// Please see the original article here for more details:
// http://www.codeproject.com/Articles/190833/Circular-Values-Math-and-Statistics-with-Cplusplus
template<typename T>
T Mod(T x, T y)
{
    static_assert(!std::numeric_limits<T>::is_exact , "Mod: floating-point type expected");

    if (0 == y)
        return x;

    double m= x - y * floor(x/y);

    // handle boundary cases resulting from floating-point limited accuracy:

    if (y > 0)              // modulo range: [0..y)
    {
        if (m>=y)           // Mod(-1e-16             , 360.    ): m= 360.
            return 0;

        if (m<0 )
        {
            if (y+m == y)
                return 0  ; // just in case...
            else
                return y+m; // Mod(106.81415022205296 , _TWO_PI ): m= -1.421e-14
        }
    }
    else                    // modulo range: (y..0]
    {
        if (m<=y)           // Mod(1e-16              , -360.   ): m= -360.
            return 0;

        if (m>0 )
        {
            if (y+m == y)
                return 0  ; // just in case...
            else
                return y+m; // Mod(-106.81415022205296, -_TWO_PI): m= 1.421e-14
        }
    }

    return m;
 }


#endif /* MISC_H_ */
