/*
 * enumerations.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 * The file for storing enumerations used in this project.
 */

#ifndef ENUMERATIONS_H_
#define ENUMERATIONS_H_

enum eFeatureTypes
{
	GlobalShader,
	SurfaceFeature
};

enum ePositionTypes
{
	XY,
	Orbit
};

// A list of shaders
// TODO: Move this information to an external file, potential organization:
//       shader_name n_params param_1_name param_2_name ... param_n_name
enum eGLShaders
{
	LD_Hesteroffer1997
};

#endif /* ENUMERATIONS_H_ */
