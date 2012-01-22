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

/// Enumerated Model Names.
/// Note, when adding a new model, list it in this enum and add it to functions:
///       GetList_AllModels() and in GetNewModel().  We want to change this, see issue #50.
enum eModels
{
	MDL_SPHERE,
	MDL_CYLINDER
};

#endif /* ENUMERATIONS_H_ */
