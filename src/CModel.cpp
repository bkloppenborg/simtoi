/*
 * CModel.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
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
 */
 
 /*
 *  Base class for all models implementing a common set of functions to get/set
 *  parameters and use shaders.
 *
 *  NOTE: When deriving from this object the yaw, pitch, and roll occupy the first
 *  three values in mParameters.
 */

#include "CModel.h"
// Header files for position objects.
#include "CPosition.h"
#include "CPositionFactory.h"
#include "CShaderFactory.h"

CModel::CModel(int n_params)
	: CParameters(4 + n_params)
{
	mBaseParams = 3;	// Number of base params, less one (zero indexed).

	// Shader storage location, boolean if it is loaded:
	mShader = CShaderPtr();

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

}

/// \brief Sets the color for the vertex based upon the model color.
void CModel::Color()
{
	glColor4d(mParams[3], 0.0, 0.0, 1.0);
}

/// \brief Static functionw which creates a lookup table of sine and cosine values
/// 	used in drawing things in polar coordinates.
///
/// Code taken from
/// <a href="http://openglut.cvs.sourceforge.net/viewvc/openglut/openglut/lib/src/og_geometry.c">OpenGLUT</a>
void CModel::CircleTable( double * sint, double * cost, const int n )
{
    int i;
    const int size = abs( n );
    double angle;

    assert( n );
    angle = 2 * PI / ( double )n;

    for( i = 0; i < size; i++ )
    {
        sint[ i ] = sin( angle * i );
        cost[ i ] = cos( angle * i );
    }

    /* Last sample is duplicate of the first */
    sint[ size ] = sint[ 0 ];
    cost[ size ] = cost[ 0 ];
}

/// \brief Returns the type of the model
string CModel::GetID()
{
	return "model_base_invalid";
}

/// \brief Returns the number of free parameters in the model
int CModel::GetNModelFreeParameters()
{
	return mNFreeParams;
}

/// \brief Returns the number of free parameters in the positioning model
int CModel::GetNPositionFreeParameters()
{
	if(mPosition != NULL)
		return mPosition->GetNFreeParams();

	return 0;
}

/// \brief Retursn the number of free parameters used in the shader
int CModel::GetNShaderFreeParameters()
{
	if(mShader != NULL)
		return mShader->GetNFreeParams();

	return 0;
}

/// \brief Get a copy of the position object. Note, this could be NULL.
CPositionPtr CModel::GetPosition(void)
{
	return mPosition;
}

/// \brief Get a copy of the shader object. Note, this could be NULL.
CShaderPtr CModel::GetShader(void)
{
	return mShader;
}

/// \brief Get the values of up to `n_params` parameters in this model
///
/// Returns all of the parameters used in the model including the position
/// and shader objects.
/// \param params A pre-allocated buffer for storing the parameters
/// \param n_params The size of `params`
void CModel::GetAllParameters(double * params, int n_params)
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
}

/// \brief Get a vector containing the minimum/maximum values for all free parameters.
///
/// \return A vector of pairs which contain the minimum and maximum values
/// that the free parameters are permitted to take.
vector< pair<double, double> > CModel::GetFreeParamMinMaxes()
{
	vector< pair<double, double> > tmp1;
	vector< pair<double, double> > tmp2 = GetFreeMinMaxes();
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

/// \brief Gets the nominal value for the free parameters in this model, including
/// 		the position and shader.
///
/// This function lets you get up to `n_params` nominial values for the free
/// parameters including the position and shader models.
///
/// \param params A pre-allocated buffer
/// \param n_params The size of `params`
/// \param scale_params If false, returns the parameters in a uniform
/// 		hypercube (x = [0...1]). If true, scales the values into
/// 		their native ranges (x = [param.min... param.max]).
void CModel::GetFreeParameters(double * params, int n_params, bool scale_params)
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


/// \brief Returns a vector of strings containing the names of the free parameters.
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

/// \brief Gets the total number of free parameters in the model.
int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() +
			this->GetNPositionFreeParameters() +
			this->GetNShaderFreeParameters();
}

/// \brief Runs OpenGL calls to rotate the model according to the inclination
/// 	position angle, and rotation angle (rotational phase).
void CModel::Rotate()
{
	// Rotations are implemented in the standard way, namely
	//  R_x(gamma) * R_y(beta) * R_z(alpha)
	// where gamma = pitch, beta = roll, alpha = yaw.

	glRotatef(mParams[0], 1, 0, 0);	// inclination
	glRotatef(mParams[1], 0, 1, 0); // position angle
	glRotatef(mParams[2], 0, 0, 1); // roll
	CCL_GLThread::CheckOpenGLError("CModel::Rotate()");
}

/// \brief Restores a model from SIMTOI's JSON save file.
///
/// This function inspects a top-level model block from a JSON save file
/// and restores the model parameters, position object, and shader method
/// by inspecting the `base_data`, `position_id` and `shader_id` fields.
/// Further restoration is passed on to the respective objects by sending
/// their respective JSON data blocks to the position and shader objects.
///
/// If no position is found, the default `CPositionXY` and default shader
/// are restored.
///
/// \param input JSON input corresponding to a top-level model block
/// 		(i.e. model_N) from a JSON save file.
void CModel::Restore(Json::Value input)
{
	// Restore the base parameters
	CParameters::Restore(input["base_data"]);

	auto positions = CPositionFactory::Instance();
	auto shaders = CShaderFactory::Instance();

	// Look up the name of the position model, if none is specified use "xy" by default.
	string position_id = input["position_id"].asString();
	if(position_id.size() == 0)
		position_id = "xy";

	auto position = positions.CreatePosition(position_id);
	position->Restore(input["position_data"]);
	SetPositionModel(position);

	// Find the shader
	string shader_id = input["shader_id"].asString();
	if(shader_id.size() == 0)
		shader_id = "default";

	auto shader = shaders.CreateShader(shader_id);
	shader->Restore(input["shader_data"]);
	SetShader(shader);
}

/// \brief Sets up the matrix mode for rendering models.
void CModel::SetupMatrix()
{
    // Rotate from (x,y,z) to (North, East, Away).  Note, we are following the (x,y,z)
    // convention of the orbital equations here.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90, 0, 0, 1);
	glScalef(1, 1, -1);

}

/// \brief Serializes a model object into a JSON object.
///
/// This function creates a Json::Value object containing six primary elements:
/// `[base|position|shader]_id` : the unique IDs of the model, position, and shader models.
///	`[base|position|shader]_data' : a serialized copy of the model, position, and shader data.
///
/// If this method is overridden, the programmer is responsible for re-creating this functionality.
///
/// \return A serialized copy of this object in JSON format.
Json::Value CModel::Serialize()
{
	Json::Value output;
	output["base_id"] = GetID();
	output["base_data"] = CParameters::Serialize();

	output["position_id"] = mPosition->GetID();
	output["position_data"] = mPosition->Serialize();

	output["shader_id"] = mShader->GetID();
	output["shader_data"] = mShader->Serialize();

	return output;
}

/// \brief Sets the inclination and position angle relative to dynamic
///		position model parameters.
///
/// If the position models are dynamic, they are often determined from
/// orbits. This function simplifies the setting of the position angle
/// and inclination to be relative to their orbital counterparts.
void CModel::SetAnglesFromPosition()
{
	if(mPosition == NULL)
		return;

	if(mPosition->GetPositionType() == CPosition::DYNAMIC)
	{
		// the inclination
		if(!this->IsFree(0))
			mParams[0] += mPosition->GetParam(0);

		// position angle
		if(!this->IsFree(1))
			mParams[1] += mPosition->GetParam(1);
	}
}

/// \brief Sets the free parameters for the model, position, and shader objects.
///
/// Iteratively sets the free parameters for the model, position, and shader
/// objects by calling `SetFreeParams()` with in_params and pointer arithmetic.
///
/// \param in_params The array of input parameters
/// \param n_params The size of `in_params`
/// \param scale_params True if `in_params` is on a unit hypercube, false otherwise.
void CModel::SetFreeParameters(double * in_params, int n_params, bool scale_params = false)
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

	// Now copy angles over from the orbit object (if they are not free)
	SetAnglesFromPosition();
}

/// \brief Assigns the position object from a position id
///
/// \param position_id A registered position ID in `CPositionFactory`
void CModel::SetPositionModel(string position_id)
{
	auto factory = CPositionFactory::Instance();
	SetPositionModel( factory.CreatePosition(position_id));
}

/// \brief Sets the position model for this objects.
///
/// \param position A valid `shared_ptr<CPosition>` object.
void CModel::SetPositionModel(CPositionPtr position)
{
	mPosition = position;
}

/// \brief Sets the time at which the model should be rendered.
void CModel::SetTime(double time)
{
	if(mPosition == NULL)
		return;

	mPosition->SetTime(time);
}

/// \brief Replaces the loaded shader with the one specified by shader_id
void CModel::SetShader(string shader_id)
{
	auto shaders = CShaderFactory::Instance();
	SetShader(shaders.CreateShader(shader_id));
}

/// \brief Replaces the loaded shader with the specified shader
///
/// \param shader A valid `shared_ptr<CShader>` object.
void CModel::SetShader(CShaderPtr shader)
{
	mShader = shader;
}

/// \brief Translates the model to the (x,y,z) position specified by the position object.
void CModel::Translate()
{
	double x, y, z;
	mPosition->GetXYZ(x, y, z);

	// Call the translation routines.  Use the double-precision call.
	glTranslatef(x, y, z);
	CCL_GLThread::CheckOpenGLError("CModel::Translate()");
}

/// \brief Use the shader on the model
///
/// Calls the shader, passing the minimum/maximum XYZ positions
/// of the verticies in the model.
///
/// \param min_xyz The minimum (x,y,z) values in the model.
/// \param max_xyz The maximum (x,y,z) values in the model.
void CModel::UseShader(double min_xyz[3], double max_xyz[3])
{
	if(mShader != NULL)
		mShader->UseShader(min_xyz, max_xyz);

	CCL_GLThread::CheckOpenGLError("CModel::UseShader()");
}
