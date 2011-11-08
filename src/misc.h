/*
 * misc.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef MISC_H_
#define MISC_H_

// Assigns input values to the output parameters using a boolean mask
void pull_params(double * in_params, int n_in_params, double * out_params, int n_out_params, bool * mask);

#endif /* MISC_H_ */
