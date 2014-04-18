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
	mTextureID = 0;

	// Init the yaw, pitch, and roll to be zero and fixed.  Set their names:
	mParamNames.push_back("Pos. Angle");
	SetParam(1, 0);
	SetFree(1, false);
	mParamNames.push_back("Inclination");
	SetParam(0, 0);
	SetFree(0, false);
	mParamNames.push_back("Rotation");
	SetParam(2, 0);
	SetFree(2, false);
	mParamNames.push_back("Color");
	SetParam(3, 1.0);
	SetFree(3, false);
}

CModel::~CModel()
{
	// Delete the texture if one was allocated.
	if(mTextureID)
	{
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);
		glDeleteTextures(1, &mTextureID);
	}
}

/// \brief Static function which creates a lookup table of sine and cosine values
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

// \brief Gets the step sizes for the free parameters.
void CModel::GetFreeParameterSteps(double * steps, unsigned int size)
{
	int n = 0;
	GetFreeParamSteps(steps, size);
	n += this->mNFreeParams;
	mPosition->GetFreeParamSteps(steps + n, size - n);
	n += mPosition->GetNFreeParams();

	if(mShader != NULL)
	{
		mShader->GetFreeParamSteps(steps + n, size - n);
		n += mShader->GetNFreeParams();
	}
}

/// \brief Gets the total number of free parameters in the model.
int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() +
			this->GetNPositionFreeParameters() +
			this->GetNShaderFreeParameters();
}

/// Initalizes the shader variables `position`, `normal`, and `tex_coords`
/// following the default packing scheme of:
///		[vec3(x,y,z), vec3(n_x, n_y, n_z), (t_x, t_y, t_z)]
/// where (x,y,z) is the position, (n_x, n_y, n_z) are the normals and
/// (t_x, t_y, t_z) are the texture coordinates.
/// All values are assumed to be floating point values.
void CModel::InitShaderVariables()
{
	// Next we need to define the storage format for this object for the shader.
	// First get the shader and activate it
	GLuint shader_program = mShader->GetProgram();
	glUseProgram(shader_program);

	// Define the storage format for the VBO. Each vertex is defined by
	// three vec3s that are packed as follows:
	// [vec3(x,y,z), vec3(n_x, n_y, n_z), (t_x, t_y, t_z)]
	// First we define the position attribute:
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(GLvoid *) 0);
	glEnableVertexAttribArray(posAttrib);
	// Setup the normals. Their offset is (3 * sizeof(float)) from the start
	// of the current index
	GLint normAttrib = glGetAttribLocation(shader_program, "normal");
	glEnableVertexAttribArray(normAttrib);
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(float), (GLvoid *) (3 * sizeof(float)));

	// Setup the texture coordinate lookup variables. Their offset
	// is (6 * sizeof(float)) from the start of the current index
	GLint texAttrib = glGetAttribLocation(shader_program, "tex_coords");
	glVertexAttribPointer(texAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(GLvoid *) (6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);

	// Setup the texture sampler. Have it use texture sampler Unit 0.
	GLuint TextureSamp = glGetUniformLocation(shader_program, "TexSampler");
	glUniform1i(TextureSamp, 0); // Set "TexSampler" to user texture Unit 0

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CModel.InitShaderVariables()");
}

/// Initializes the texture by setting properties and uploading a default
/// (smooth gradient) texture to the texture buffer.
void CModel::InitTexture()
{
	glActiveTexture(GL_TEXTURE0);

	// Create a new texture of type GL_TEXTURE_RECTANGLE and bind to it.
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE, mTextureID);

	// Load a default (smooth graident) texture into the texture buffer
	for(unsigned int i = 0; i < mTexture.size(); i++)
	{
		mTexture[i].x = float(i) / mTexture.size();
		mTexture[i].a = 1.0;
	}
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mTexture[0]);

	// Set wrapping and lookup filters
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// All done. Unbind.
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	CWorkerThread::CheckOpenGLError("CModel.InitTexture(), texture setup");
}

/// \brief Constructs the rotation matrix according to the set parameters.
///
/// Note, if the position model is DYNAMIC (i.e. an orbit) the position angle
/// of the model is specified relative to the orbital plane.
glm::mat4 CModel::Rotate()
{
	double Omega = mParams[0];
	double inc = mParams[1];
	double omega = mParams[2];

	// If we have a dynamic position, simply add the angles
	if(mPosition->GetPositionType() == CPosition::DYNAMIC)
	{
		double orbit_Omega = mPosition->GetParam(0);
		double orbit_inc = mPosition->GetParam(1);
		double orbit_omega = mPosition->GetParam(2);

		Omega += orbit_Omega;
		inc += orbit_inc;
		omega += orbit_omega;
	}

	glm::mat4 A = glm::rotate(mat4(), float(Omega), vec3(0.0, 0.0, 1.0));
	glm::mat4 B = glm::rotate(mat4(), float(inc), vec3(1.0, 0.0, 0.0));
	glm::mat4 C = glm::rotate(mat4(), float(omega), vec3(0.0, 0.0, 1.0));
	return A * B * C;
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
	CModel::SetPositionModel(position);

	// Find the shader
	string shader_id = input["shader_id"].asString();
	if(shader_id.size() == 0)
		shader_id = "default";

	auto shader = shaders.CreateShader(shader_id);
	shader->Restore(input["shader_data"]);
	CModel::SetShader(shader);
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

/// Computes the flux for pixels given the input temperatures following Planck's
/// law.
/// @param temperatures An array of temperatures of in Kelvin
/// @param fluxes Array into which computed fluxes will be stored
/// @param wavelength The wavelength of observation
/// @param max_temperature The maximum temperature of all models (used for normalization)
void CModel::TemperatureToFlux(vector<double> temperatures, vector<float> & fluxes,
		double wavelength, double max_temperature)
{
	// The pixel and temperature buffers must be of the same size.
	assert(fluxes.size() == temperatures.size());

	// Planck's law:
	// B(lambda, T) propto  1 / {exp[(h*c/k)/(lambda*T)] - 1}
	// h*c/k = 0.0143877696 m K
	double max_flux = 1. / (exp(0.0143877696 / (wavelength * max_temperature)) - 1.);
	for (unsigned int i = 0; i < temperatures.size(); i++)
	{
		fluxes[i] = 1. / (exp(0.0143877696 / (wavelength * temperatures[i])) - 1.);

		fluxes[i] /= max_flux;
	}

}

/// Computes the flux for pixels given the input temperatures following Planck's
/// law.
/// @param temperatures An array of temperatures of in Kelvin
/// @param pixels A vector of RGBA pixels into which the computes fluxes will be stored
/// @param wavelength The wavelength of observation
/// @param max_temperature The maximum temperature of all models (used for normalization)
void CModel::TemperatureToFlux(vector<double> temperatures, vector<vec4> & fluxes,
		double wavelength, double max_temperature)
{
	// The pixel and temperature buffers must be of the same size.
	assert(fluxes.size() == temperatures.size());

	// Planck's law:
	// B(lambda, T) propto  1 / {exp[(h*c/k)/(lambda*T)] - 1}
	// h*c/k = 0.0143877696 m K
	double max_flux = 1. / (exp(0.0143877696 / (wavelength * max_temperature)) - 1.);
	for (unsigned int i = 0; i < temperatures.size(); i++)
	{
		// fill in the Red component.
		fluxes[i].r = 1. / (exp(0.0143877696 / (wavelength * temperatures[i])) - 1.);

		fluxes[i].r /= max_flux;
	}

}

/// \brief Constructs the translation matrix from the position model.
glm::mat4 CModel::Translate()
{
	double x, y, z;
	mPosition->GetXYZ(x, y, z);

	return glm::translate(mat4(), vec3(x, y, z));
}
