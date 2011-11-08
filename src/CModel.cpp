/*
 * CModel.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CModel.h"
// Header files for position objects.
#include "CPosition.h"
#include "CPositionXY.h"

CModel::CModel()
{
	// Init the object to have no rotation in yaw, pitch, or roll.
	rotation[0] = rotation[1] = rotation[2] = 0;

	position = new CPositionXY();
}

CModel::~CModel()
{
	// Free up memory.
	delete position;
}

void CModel::Rotate()
{
	glRotated(rotation[0], 1, 0, 0);
	glRotated(rotation[1], 0, 1, 0);
	glRotated(rotation[2], 0, 0, 1);
}

void CModel::Translate()
{
	double x, y, z;
	position->GetXYZ(x, y, z);

	// Call the translation routines.  Use the double-precision call.
	glTranslated(x, y, z);
}

void CModel::SetPositionType(ePositionTypes type)
{
	if(position->GetType() != type)
	{
		delete position;

		if(type == XY)
			position = new CPositionXY();

		// TODO: Implement additional position types.
	}
}
