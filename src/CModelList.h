/*
 * CModelList.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CMODELLIST_H_
#define CMODELLIST_H_

#include <vector>

#include "COpenGL.h"
#include "CModel.h"


using namespace std;

// A container for a list of models.
class CModelList
{
	vector<CModel*> models;

private:
	bool SortModelPredicate(CModel * A, CModel * B);

public:
	CModelList();
	~CModelList();

	void Append(CModel* model);

	void GetParameters(float * params, int n_params);

	void Render(COpenGL * gl);

	void SetParameters(float * params, int n_params);

};

#endif /* CMODELLIST_H_ */
