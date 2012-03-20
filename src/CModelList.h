/*
 * CModelList.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  A Class for storing and loading models.
 */

#ifndef CMODELLIST_H_
#define CMODELLIST_H_

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include "CVectorList.h"
#include "enumerations.h"
#include "CPosition.h"

class CModel;
class CGLShaderWrapper;
class CGLShaderList;

using namespace std;



// A container for a list of models.
class CModelList : public CVectorList<CModel*>
{
public:
	/// Enumerated Model Names.
	/// Note, when adding a new model, list it in this enum and add it to functions:
	///       GetTypes() and in GetNewModel().  We want to change this, see issue #50.
	enum ModelTypes
	{
		NONE,
		SPHERE,
		DISK,
		DISK_A,
		DISK_B,
		DISK_C,
		LAST_VALUE // must be the last value in this list.
	};

protected:
	double mTime;
	double mTimestep;

public:
	CModelList();
	~CModelList();

	CModel * AddNewModel(ModelTypes model_id);

	int GetNFreeParameters();
	void GetAllParameters(double * params, int n_params);
	vector< pair<double, double> > GetFreeParamMinMaxes();
	void GetFreeParameters(double * params, int n_params, bool scale_params);
	void GetFreeParametersScaled(double * params, int n_params);
	double GetFreeParameterPriorProduct();
	vector<string> GetFreeParamNames();
	CModel * GetModel(int i) { return mList[i]; };
	double GetTime() { return mTime; };

	static vector< pair<ModelTypes, string> > GetTypes(void);

	void IncrementTime();

	void Render(GLuint fbo, int width, int height);
	void Restore(Json::Value input, CGLShaderList * shader_list);

	Json::Value Serialize();
	void SetFreeParameters(double * params, unsigned int n_params, bool scale_params);
	void SetPositionType(unsigned int model_id, CPosition::PositionTypes pos_type);
	void SetShader(unsigned int model_id, CGLShaderWrapper * shader);
	void SetTime(double t);
	void SetTimestep(double dt);

};

#endif /* CMODELLIST_H_ */
