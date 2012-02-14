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

vector< pair<float, float> > CModel::GetFreeParamMinMaxes()
{
	vector< pair<float, float> > tmp1;
	vector< pair<float, float> > tmp2 = GetFreeMinMaxes();
	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	tmp2 = mPosition->GetFreeMinMaxes();
	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );

	if(mShader != NULL)
	{
		tmp2 = mShader->GetFreeMinMaxes();
		tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	}

	return tmp1;
}

/// Gets the free parameters for this model in a:
///  scale_params = false => uniform hypercube (x = [0...1])
///  scale_params = true => native values (x = [param.min... param.max])
void CModel::GetFreeParameters(float * params, int n_params, bool scale_params)
{
	int n = 0;
	GetFreeParams(params, n_params, scale_params);
	n += this->mNFreeParams;
	mPosition->GetFreeParams(params + n, n_params - n, scale_params);
	n += mPosition->GetNFreeParams();

	if(mShader != NULL)
	{
		mShader->GetFreeParams(params + n, n_params - n, scale_params);
		n += mShader->GetNFreeParams();
	}

}


/// Returns a vector of strings containing the names of the free parameters:
vector<string> CModel::GetFreeParameterNames()
{
	vector<string> tmp1;
	vector<string> tmp2 = GetFreeParamNames();
	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	tmp2 = mPosition->GetFreeParamNames();
	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );

	if(mShader != NULL)
	{
		tmp2 = mShader->GetFreeParamNames();
		tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	}

	return tmp1;
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
	CCL_GLThread::CheckOpenGLError("CModel::Rotate()");
}

void CModel::Restore(Json::Value input, CGLShaderList * shader_list)
{
	// Restore the base parameters
	CParameters::Restore(input["base"]);
	CGLShaderWrapper * shader;

	// Now the position
	if(input.isMember("position"))
	{
		if(input["position"].isMember("type"))
		{
			SetPositionType( CPosition::PositionTypes(input["position"]["type"].asInt()) );
			mPosition->Restore(input["position"]);
		}
	}

	// Now the shader
	if(input.isMember("shader"))
	{
		if(input["shader"].isMember("type"))
		{
			shader = shader_list->GetShader( CGLShaderList::ShaderTypes( input["shader"]["type"].asInt()) );
			SetShader(shader);
			mShader->Restore(input["shader"]);
		}
	}
}

/// Sets up the matrix mode for rendering models.
void CModel::SetupMatrix()
{
	// Load the matrix mode, and identity, then flip x -> -x to correspond with astronomical notation.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(-1, 1, 1);

}

/// Serializes a model object into a JSON object.
Json::Value CModel::Serialize()
{
	Json::Value output;
	output["base"] = CParameters::Serialize();
	output["base"]["type"] = mType;
	output["position"] = mPosition->Serialize();
	output["position"]["type"] = mPosition->GetType();

	if(mShader != NULL)
	{
		output["shader"] = mShader->Serialize();
		output["shader"]["type"] = mShader->GetType();
	}

	return output;
}

/// Sets the parameters for this model, the position, shader, and all features.
void CModel::SetFreeParameters(float * in_params, int n_params, bool scale_params)
{
	// Here we use pointer math to advance the position of the array passed to the functions
	// that set the parameters.  First assign values to this model (use pull_params):
	int n = 0;
	SetFreeParams(in_params, n_params, scale_params);
	n += mNFreeParams;
	// Now set the values for the position object
	mPosition->SetFreeParams(in_params + n, n_params - n, scale_params);
	n += mPosition->GetNFreeParams();
	// Then the shader.
	if(mShader != NULL)
	{
		mShader->SetFreeParams(in_params + n, n_params - n, scale_params);
		n += mShader->GetNFreeParams();
	}
	// Lastly the features
	//features->SetParams(in_params + n, n_params - n);
}

/// Assigns and initializes a position type.
void CModel::SetPositionType(CPosition::PositionTypes type)
{
	// If the position is already set and is of the current type, break.
	if(mPosition != NULL && mPosition->GetType() == type)
		return;

	mPosition = CPosition::GetPosition(type);
}

void CModel::SetTime(double time)
{
	CPositionOrbit * tmp;
	if(mPosition->GetType() == CPosition::ORBIT)
	{
		tmp = reinterpret_cast<CPositionOrbit*>(mPosition);
		tmp->SetTime(time);
	}
}

void CModel::SetShader(CGLShaderWrapper * shader)
{
	mShader = shader;
}

void CModel::Translate()
{
	float x, y, z;
	mPosition->GetXYZ(x, y, z);

	// Call the translation routines.  Use the double-precision call.
	glTranslatef(x, y, z);
	CCL_GLThread::CheckOpenGLError("CModel::Translate()");
}

void CModel::UseShader()
{
	if(mShader != NULL)
		mShader->UseShader();

	CCL_GLThread::CheckOpenGLError("CModel::UseShader()");
}
