/*
 * CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */


#include "CModelList.h"

using namespace std;

CModelList::CModelList()
{
	// TODO Auto-generated constructor stub

}

CModelList::~CModelList()
{
	// Deallocate the models
	for(int i = models.size() - 1; i > -1; i--)
		delete models[i];
}

void CModelList::Append(CModel * model)
{
	this->models.push_back(model);
}

// Creates a flattened image of the specified dimentions from the models contained in this object.
void CModelList::Render()
{

}

// This function sets the parameters for models prior to a GetData call.
void CModelList::SetModelParams(double * params, int n_params)
{

}

void CModelList::GetData()
{
}
