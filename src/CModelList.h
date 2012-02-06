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

using namespace std;

// A container for a list of models.
class CModelList : public CVectorList<CModel*>
{
protected:
	double mTime;
	double mTimestep;

public:
	CModelList();
	~CModelList();

	CModel * AddNewModel(eModels model_id);

	int GetNFreeParameters();
	void GetAllParameters(float * params, int n_params);
	void GetFreeParameters(float * params, int n_params);
	CModel * GetModel(int i) { return mList[i]; };
	double GetTime() { return mTime; };

	vector< pair<eModels, string> > GetList_AllModels(void);

	void IncrementTime();

	void Render(GLuint fbo, int width, int height);

	void SetFreeParameters(float * params, int n_params);
	void SetPositionType(int model_id, ePositionTypes pos_type);
	void SetShader(int model_id, CGLShaderWrapper * shader);
	void SetTime(double t);
	void SetTimestep(double dt);

};

#endif /* CMODELLIST_H_ */
