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

public:
	CModelList();
	~CModelList();

	CModel * AddNewModel(eModels model_id);

	int GetNFreeParameters();
	void GetParameters(float * params, int n_params);
	CModel * GetModel(int i) { return mList[i]; };

	vector< pair<eModels, string> > GetList_AllModels(void);

	void Render(GLuint fbo, int width, int height);

	void SetParameters(float * params, int n_params);
	void SetPositionType(int model_id, ePositionTypes pos_type);
	void SetShader(int model_id, CGLShaderWrapper * shader);

};

#endif /* CMODELLIST_H_ */
