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

// An enumeration of shaders
// TODO: we need to have a simple way of mapping
enum eGLShaders
{
	LD_Hesteroffer1997
};

#endif /* ENUMERATIONS_H_ */
