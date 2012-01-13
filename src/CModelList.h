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

#include <vector>

class CModel;

/// Enumerated Model Names.
/// Note, when adding a new model, list it in this enum and add it to functions:
///       GetList_AllModels() and in GetNewModel().  We want to change this, see issue #50.
enum eModels
{
	MDL_SPHERE
};

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

	int GetNFreeParameters();
	void GetParameters(float * params, int n_params);

	vector< pair<string, eModels> > GetList_AllModels(void);

	CModel * GetNewModel(eModels);

	void Render(GLuint fbo, int width, int height);

	void SetParameters(float * params, int n_params);

};

#endif /* CMODELLIST_H_ */
