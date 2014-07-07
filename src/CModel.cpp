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

#include <sstream>
#include <stdexcept>

using namespace std;

#include "CModel.h"
// Header files for position objects.
#include "CPosition.h"
#include "CPositionFactory.h"
#include "CShaderFactory.h"
#include "CFeature.h"
#include "CFeatureFactory.h"
#include "misc.h"

CModel::CModel()
{
	// Shader storage location, boolean if it is loaded:
	mShader = CShaderPtr();
	mFluxTextureID = 0;
	mTime = 0;

	addParameter("position_angle", 0, 0, 360, false, 0.1, "Position angle",
			"Position Angle defined from North rotating East (degrees)");
	addParameter("inclination", 0, -180, 180, false, 1.0, "Inclination",
			"Inclination defined from the plane of the sky (degrees)");
	addParameter("z_axis_rotation", 0, 0, 360, false, 1.0, "Rotation zero point",
			"Initial rotation angle about model's internal z-axis (degrees). Unless you have a specific reason, this should be zero.");
	addParameter("z_axis_rotational_period", 0, 0, 100, false, 1, "Rotational period",
			"Rotational period about the model'z z-axis (days)");
}

CModel::~CModel()
{
	// Delete the texture if one was allocated.
	if(mFluxTextureID)
	{
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);
		glDeleteTextures(1, &mFluxTextureID);
	}
}

/// \breif Adds the feature with feature_id to the current model.
void CModel::AddFeature(string feature_id)
{
	auto features = CFeatureFactory::Instance();
	auto feature = features.CreateFeature(feature_id);

	if(feature != nullptr)
		mFeatures.push_back(feature);
}

/// \breif Function for finding the IDs of pixels within the bounds
/// (s0, s1, s2) +/- (ds0, ds1, ds2)
/// where (s0, s1, s2) are the generalized coordinates in the model's
/// coordinate system.
void CModel::FindPixels(double s0, double s1, double s2,
		double ds0, double ds1, double ds2,
		vector<unsigned int> &pixels_ids)
{
	// base class function cannot match pixels. Do nothing here.
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

const vector<CFeaturePtr> & CModel::GetFeatures() const
{
	return mFeatures;
};

/// \brief Returns the number of free parameters used by all features
int CModel::GetNFeatureFreeParameters()
{
	unsigned int n_free = 0;

	for(auto feature: mFeatures)
		n_free += feature->getFreeParameterCount();

	return n_free;
}

/// \brief Returns the number of free parameters in the model
int CModel::GetNModelFreeParameters()
{
	unsigned int n_free = 0;
	for(auto parameter: mParams)
	{
		if(parameter.second.isFree())
			n_free++;
	}

	return n_free;
}

/// \brief Returns the number of free parameters in the positioning model
int CModel::GetNPositionFreeParameters()
{
	if(mPosition != NULL)
		return mPosition->getFreeParameterCount();

	return 0;
}

/// \brief Returns the number of free parameters used in the shader
int CModel::GetNShaderFreeParameters()
{
	if(mShader != NULL)
		return mShader->getFreeParameterCount();

	return 0;
}

/// \brief Get a reference to the pixel temperatures
vector<double> & CModel::GetPixelTemperatures()
{
	return mPixelTemperatures;
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
	// Current parameter number
	unsigned int n = 0;
	// Get this object's parameters
	CParameterMap::getAllParameters(params + n, n_params - n);

	// Get parameters from the position object.
	n += mPosition->getAllParameters(params + n, n_params - n);

	if(mShader != NULL)
		n += mShader->getAllParameters(params + n, n_params - n);

	for(auto feature: mFeatures)
		n += feature->getAllParameters(params + n, n_params - n);
}

/// \brief Get a vector containing the minimum/maximum values for all free parameters.
///
/// \return A vector of pairs which contain the minimum and maximum values
/// that the free parameters are permitted to take.
vector< pair<double, double> > CModel::GetFreeParamMinMaxes()
{
	vector< pair<double, double> > min_maxes;
	vector< pair<double, double> > tmp1;

	tmp1 = CParameterMap::getFreeParameterMinMaxes();
	min_maxes.insert( min_maxes.end(), tmp1.begin(), tmp1.end() );

	tmp1 = mPosition->getFreeParameterMinMaxes();
	min_maxes.insert( min_maxes.end(), tmp1.begin(), tmp1.end() );

	if(mShader != NULL)
	{
		tmp1 = mShader->getFreeParameterMinMaxes();
		min_maxes.insert( min_maxes.end(), tmp1.begin(), tmp1.end() );
	}

	for(auto feature: mFeatures)
	{
		tmp1 = feature->getFreeParameterMinMaxes();
		min_maxes.insert( tmp1.end(), tmp1.begin(), tmp1.end() );
	}

	return min_maxes;
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
	unsigned int n = 0;

	CParameterMap::getFreeParameters(params, n_params, scale_params);
	n += getFreeParameterCount();

	n += mPosition->getFreeParameters(params + n, n_params - n, scale_params);

	if(mShader != NULL)
		n += mShader->getFreeParameters(params + n, n_params - n, scale_params);

	for(auto feature: mFeatures)
		n += feature->getFreeParameters(params + n, n_params - n, scale_params);
}


/// \brief Returns a vector of strings containing the names of the free parameters.
vector<string> CModel::GetFreeParameterNames()
{
	vector<string> tmp1;
	vector<string> tmp2;

	for(auto it: mParams)
	{
		if(it.second.isFree())
		{
			string name = it.second.getHumanName();
			tmp1.push_back(name);
		}
	}

	tmp2 = mPosition->getFreeParameterNames();
	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );

	if(mShader != NULL)
	{
		tmp2 = mShader->getFreeParameterNames();
		tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	}

	for(auto feature: mFeatures)
	{
		tmp2 = feature->getFreeParameterNames();
		tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
	}

	return tmp1;
}

// \brief Gets the step sizes for the free parameters.
void CModel::GetFreeParameterSteps(double * steps, unsigned int size)
{
	unsigned int n = 0;

	n += CParameterMap::getFreeParameterStepSizes(steps, size);
	n += mPosition->getFreeParameterStepSizes(steps + n, size - n);

	if(mShader != NULL)
		n += mShader->getFreeParameterStepSizes(steps + n, size - n);

	for(auto feature: mFeatures)
		n += feature->getFreeParameterStepSizes(steps + n, size - n);
}

/// \brief Gets the total number of free parameters in the model.
int CModel::GetTotalFreeParameters()
{
	// Sum up the free parameters from the model, position, and features
	return this->GetNModelFreeParameters() +
			this->GetNPositionFreeParameters() +
			this->GetNShaderFreeParameters() +
			this->GetNFeatureFreeParameters();
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
	// If the normal shader variable is not used, it can be optimized out
	// verify that it is in use before setting any properties..
	if(normAttrib > -1)
	{
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,
				3 * sizeof(float), (GLvoid *) (3 * sizeof(float)));
	}

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
	glGenTextures(1, &mFluxTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);

	// Load a default (smooth graident) texture into the texture buffer
	for(unsigned int i = 0; i < mFluxTexture.size(); i++)
	{
		mFluxTexture[i].r = float(i) / mFluxTexture.size();
		mFluxTexture[i].a = 1.0;
	}
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mFluxTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mFluxTexture[0]);

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
	double Omega = mParams["position_angle"].getValue() * M_PI / 180;
	double inc = mParams["inclination"].getValue() * M_PI / 180;
	double omega = mParams["z_axis_rotation"].getValue() * M_PI / 180;

	// If we have a dynamic position, simply add the angles
	if(mPosition->GetPositionType() == CPosition::DYNAMIC)
	{
		double orbit_Omega = mPosition->getParameter("Omega").getValue() * M_PI / 180;
		double orbit_inc = mPosition->getParameter("inclination").getValue() * M_PI / 180;
		double orbit_omega = mPosition->getParameter("omega").getValue() * M_PI / 180;

		Omega += orbit_Omega;
		inc += orbit_inc;
		omega += orbit_omega;
	}

	glm::mat4 A = glm::rotate(mat4(), float(Omega), vec3(0.0, 0.0, 1.0));
	glm::mat4 B = glm::rotate(mat4(), float(inc),   vec3(1.0, 0.0, 0.0));
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
	CParameterMap::restore(input["base_data"]);

	auto positions = CPositionFactory::Instance();
	auto shaders = CShaderFactory::Instance();
	auto features = CFeatureFactory::Instance();

	// Look up the name of the position model, if none is specified use "xy" by default.
	string position_id = input["position_id"].asString();
	if(position_id.size() == 0)
		position_id = "xy";

	auto position = positions.CreatePosition(position_id);
	position->restore(input["position_data"]);
	CModel::SetPositionModel(position);

	// Find the shader
	string shader_id = input["shader_id"].asString();
	if(shader_id.size() == 0)
		shader_id = "default";

	auto shader = shaders.CreateShader(shader_id);
	shader->restore(input["shader_data"]);
	CModel::SetShader(shader);

	// Lastly restore features:
	unsigned int i = 0;
	string feature_id;
	stringstream temp;
	while(true)
	{
		// Attempt to restore the shader, if this fails it's ok.
		try
		{
			// Look up the ID
			temp.clear();
			temp.str(std::string());
			temp << "feature_" << i << "_id";
			feature_id = input[temp.str()].asString();

			// If there was no string found, quit searching.
			if(feature_id.size() == 0)
				break;

			// Find the feature
			auto feature = features.CreateFeature(feature_id);
			// See if the feature was found, if not print out an error
			if(feature == nullptr)
			{
				throw runtime_error("The feature with ID '" + feature_id + "' not registered with CFeatureFactory");
				break;
			}

			temp.clear();
			temp.str(std::string());
			temp << "feature_" << i << "_data";
			feature->restore(input[temp.str()]);

			// The feature is restored, add it to the list.
			mFeatures.push_back(feature);

			// increment the feature counter
			i++;
		}
		catch(...)
		{
			break;
		}
	}
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
	output["base_id"] = getID();
	output["base_data"] = CParameterMap::serialize();

	output["position_id"] = mPosition->getID();
	output["position_data"] = mPosition->serialize();

	output["shader_id"] = mShader->getID();
	output["shader_data"] = mShader->serialize();

	stringstream temp;

	// Serialize each feature ID as
	//  "feature_id_#" -> feature_id
	//  "feature_data_#" -> seralized data for the feature
	for(unsigned int i = 0; i < mFeatures.size(); i++)
	{
		auto feature = mFeatures[i];

		temp.clear();
		temp.str(std::string());
		temp << "feature_" << i << "_id";
		output[temp.str()] = feature->getID();
		temp.clear();
		temp.str(std::string());
		temp << "feature_" << i << "_data";
		output[temp.str()] = feature->serialize();
	}

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
	unsigned int n = 0;
	// Here we use pointer math to advance the position of the array passed to the functions
	// that set the parameters.  First assign values to this model (use pull_params):
	n += CParameterMap::setFreeParameterValues(in_params, n_params, scale_params);

	// Now set the values for the position object
	n += mPosition->setFreeParameterValues(in_params + n, n_params - n, scale_params);
	// Then the shader.
	if(mShader != NULL)
		n += mShader->setFreeParameterValues(in_params + n, n_params - n, scale_params);

	for(auto feature: mFeatures)
		n += feature->setFreeParameterValues(in_params + n, n_params - n, scale_params);
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
///
/// Sets the time for the current model. Internally this updates any time-dependent
/// properties such as the position and rotation about the z-axis.
void CModel::SetTime(double time)
{
	// Call the base class method (updates orbital parameters, etc.)
	if(mPosition != NULL)
		mPosition->SetTime(time);

	// Get the rotational period (in days)
	const double rotational_period = mParams["z_axis_rotational_period"].getValue();
	const double rotation_zero_point = mParams["z_axis_rotation"].getValue();
	if(rotational_period > 0)
	{
		double omega_dot = 2 * PI / rotational_period;

		// Compute current rotation angle and set it.
		double dt = time - mTime;
		// Compute the modulus using Mod (in misc.h) rather than fmod as this function
		// is more resiliant to edge cases than fmod.
		double rotation = Mod(rotation_zero_point + omega_dot * dt, 360.0d);
		mParams["z_axis_rotation"].setValue(rotation);
	}

	// Update the current time.
	mTime = time;
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
