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
#include "CFeature.h"
#include "CFeatureList.h"

CModel::CModel()
{
	// Init the object to have no rotation in yaw, pitch, or roll.
	rotation[0] = rotation[1] = rotation[2] = 0;
	shader = NULL;
	position = new CPositionXY();
	features = new CFeatureList();
}

CModel::~CModel()
{
	// Free up memory.
	delete position;
}

int CModel::GetNPositionFreeParameters()
{
	return position->GetNFreeParameters();
}

int CModel::GetNFeatureFreeParameters()
{
	return features->GetNFreeParameters();
}

void CModel::Rotate()
{
	// Rotations are implemented in the standard way, namely
	//  R_x(gamma) * R_y(beta) * R_z(alpha)
	// where gamma = pitch, beta = roll, alpha = yaw.

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

int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() + this->GetNPositionFreeParameters() + this->GetNFeatureFreeParameters();
}

void CModel::SetParameters(double * params, int n_params)
{
	// Send parameter set command to the components of this model.
	// We use pointer math to advance the position of the array passed to the functions
	int n = 0;
	SetModelParameters(params, n_params);
	n += this->n_free_parameters;
	position->SetParams(params + n, n_params - n);
	n += position->GetNFreeParameters();

	if(shader != NULL)
	{
		shader->SetParams(params + n, n_params - n);
		n += shader->GetNFreeParams();
	}

	features->SetParams(params + n, n_params - n);
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

void CModel::SetShader(CShader * shader)
{
	this->shader = shader;
}

void CModel::UseShader()
{
	if(this->shader != NULL)
		shader->UseShader();
}
