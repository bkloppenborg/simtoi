/*
 * CModel.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  Base class for all models implementing a common set of functions to get/set
 *  parameters and use shaders.
 *
 *  NOTE: When deriving from this object the yaw, pitch, and roll occupy the first
 *  three values in mParameters.
 */

#include "CModel.h"
// Header files for position objects.
#include "CPosition.h"
#include "CPositionXY.h"
#include "CPositionOrbit.h"
//#include "CFeature.h"
//#include "CFeatureList.h"

CModel::CModel(int n_params)
	: CParameters(4 + n_params)
{
	mPosition = NULL;
	mType = CModelList::NONE;

	mBaseParams = 3;	// Number of base params, less one (zero indexed).

	// Shader storage location, boolean if it is loaded:
	mShader = NULL;
	mShaderLoaded = false;

	// Init the yaw, pitch, and roll to be zero and fixed.  Set their names:
	mParamNames.push_back("Inclination");
	SetParam(0, 0);
	SetFree(0, false);
	mParamNames.push_back("Pos. Angle");
	SetParam(1, 0);
	SetFree(1, false);
	mParamNames.push_back("Rotation");
	SetParam(2, 0);
	SetFree(2, false);
	mParamNames.push_back("Color");
	SetParam(3, 1.0);
	SetFree(3, false);
}

CModel::~CModel()
{
	// Free up memory.
	delete mPosition;
}


/// Returns the values for all parameters in this model
/// including the model, position, shader, and all features.
void CModel::GetAllParameters(float * params, int n_params)
{
	// Send parameter set command to the components of this model.
	// We use pointer math to advance the position of the array passed to the functions
	int n = 0;
	GetParams(params, n_params);
	n += this->mNParams;
	mPosition->GetParams(params + n, n_params - n);
	n += mPosition->GetNParams();

	if(mShader != NULL)
	{
		mShader->GetParams(params + n, n_params - n);
		n += mShader->GetNParams();
	}

	// TODO: Implement this function
	//features->GetParams(params + n, n_params - n);
}

/// Sets the free parameters for this model.
void CModel::GetFreeParameters(float * params, int n_params)
{
	int n = 0;
	GetFreeParams(params, n_params);
	n += this->mNFreeParams;
	mPosition->GetFreeParams(params + n, n_params - n);
	n += mPosition->GetNFreeParams();

	if(mShader != NULL)
	{
		mShader->GetFreeParams(params + n, n_params - n);
		n += mShader->GetNFreeParams();
	}

}

int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() + this->GetNPositionFreeParameters() + this->GetNShaderFreeParameters() + this->GetNFeatureFreeParameters();
}

void CModel::Color()
{
	glColor3f(mParams[3], 0.0, 0.0);
}

void CModel::Rotate()
{
	// Rotations are implemented in the standard way, namely
	//  R_x(gamma) * R_y(beta) * R_z(alpha)
	// where gamma = pitch, beta = roll, alpha = yaw.

	glRotatef(mParams[0], 1, 0, 0);	// inclination
	glRotatef(-mParams[1], 0, 1, 0); // position angle
	glRotatef(mParams[2], 0, 0, 1); // roll
}

/// Sets up the matrix mode for rendering models.
void CModel::SetupMatrix()
{
	// Load the matrix mode, and identity, then flip x -> -x to correspond with astronomical notation.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(-1, 1, 1);

}

void CModel::Translate()
{
	float x, y, z;
	mPosition->GetXYZ(x, y, z);

	// Call the translation routines.  Use the double-precision call.
	glTranslatef(x, y, z);
}

/// Sets the parameters for this model, the position, shader, and all features.
void CModel::SetFreeParameters(float * in_params, int n_params)
{
	// Here we use pointer math to advance the position of the array passed to the functions
	// that set the parameters.  First assign values to this model (use pull_params):
	int n = 0;
	SetFreeParams(in_params, n_params);
	n += mNFreeParams;
	// Now set the values for the position object
	mPosition->SetFreeParams(in_params + n, n_params - n);
	n += mPosition->GetNFreeParams();
	// Then the shader.
	if(mShader != NULL)
	{
		mShader->SetFreeParams(in_params + n, n_params - n);
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
	case POSITION_ORBIT:
		mPosition = new CPositionOrbit();
		break;

	case POSITION_XY:
	default:
		// By default models use XY position.
		mPosition = new CPositionXY();
		break;
	}
}

void CModel::SetTime(double time)
{
	CPositionOrbit * tmp;
	if(mPosition->GetType() == POSITION_ORBIT)
	{
		tmp = reinterpret_cast<CPositionOrbit*>(mPosition);
		tmp->SetTime(time);
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
