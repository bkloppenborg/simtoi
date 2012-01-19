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
//#include "CFeature.h"
//#include "CFeatureList.h"

CModel::CModel(int n_params)
	: CParameters(n_params + 3)
{
	mPosition = NULL;

	// Shader storage location, boolean if it is loaded:
	mShader = NULL;
	mShaderLoaded = false;

	// Init the yaw, pitch, and roll to be zero and fixed.  Set their names:
	mParamNames.push_back("Yaw");
	SetParam(0, 0);
	SetFree(0, false);
	mParamNames.push_back("Pitch");
	SetParam(1, 0);
	SetFree(1, false);
	mParamNames.push_back("Roll");
	SetParam(2, 0);
	SetFree(2, false);
}

CModel::~CModel()
{
	// Free up memory.
	delete mPosition;
}

int CModel::GetNPositionFreeParameters()
{
	return mPosition->GetNFreeParams();
}

int CModel::GetNFeatureFreeParameters()
{
	return 0;
	//return features->GetNFreeParameters();
}

void CModel::Rotate()
{
	// Rotations are implemented in the standard way, namely
	//  R_x(gamma) * R_y(beta) * R_z(alpha)
	// where gamma = pitch, beta = roll, alpha = yaw.

	glRotated(mParams[0], 1, 0, 0);	// yaw
	glRotated(mParams[1], 0, 1, 0); // pitch
	glRotated(mParams[2], 0, 0, 1); // roll
}

void CModel::Translate()
{
	float x, y, z;
	mPosition->GetXYZ(x, y, z);

	// Call the translation routines.  Use the double-precision call.
	glTranslated(x, y, z);
}

/// Returns the values for all parameters in this model
/// including the model, position, shader, and all features.
void CModel::GetAllParameters(float * params, int n_params)
{
	// Send parameter set command to the components of this model.
	// We use pointer math to advance the position of the array passed to the functions
	int n = 0;
	GetParams(params, n_params);
	n += this->mNFreeParams;
	mPosition->GetParams(params + n, n_params - n);
	n += mPosition->GetNFreeParams();

	if(mShader != NULL)
	{
		mShader->GetParams(params + n, n_params - n);
		n += mShader->GetNFreeParams();
	}

	// TODO: Implement this function
	//features->GetParams(params + n, n_params - n);
}

int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() + this->GetNPositionFreeParameters() + this->GetNFeatureFreeParameters();
}

int CModel::GetNModelFreeParameters()
{
	return mNFreeParams;
}

/// Sets the parameters for this model, the position, shader, and all features.
void CModel::SetAllParameters(float * in_params, int n_params)
{
	// Here we use pointer math to advance the position of the array passed to the functions
	// that set the parameters.  First assign values to this model (use pull_params):
	int n = 0;
	SetParams(in_params, n_params);
	n += mNFreeParams;
	// Now set the values for the position object
	mPosition->SetParams(in_params + n, n_params - n);
	n += mPosition->GetNFreeParams();
	// Then the shader.
	if(mShader != NULL)
	{
		mShader->SetParams(in_params + n, n_params - n);
		n += mShader->GetNFreeParams();
	}
	// Lastly the features
	//features->SetParams(in_params + n, n_params - n);
}

/// Assigns and initializes a position type.
void CModel::SetPositionType(ePositionTypes type)
{
	// If the position is already set and is of the current type, break.
	if(mPosition != NULL && mPosition->GetType() == type)
		return;

	// Otherwise assign the position.
	switch(type)
	{
//	case Orbit:
//		position = new PositionOrbit();
//		break;
	default:
		// By default models use XY position.
		mPosition = new CPositionXY();
		break;
	}
}

void CModel::SetShader(CGLShaderWrapper * shader)
{
	mShader = shader;
}

void CModel::UseShader()
{
	if(mShader != NULL)
		mShader->UseShader();
}
