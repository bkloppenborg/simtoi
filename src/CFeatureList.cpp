/*
 * CFeatureList.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "CFeatureList.h"



using namespace std;

CFeatureList::CFeatureList()
{
	// TODO Auto-generated constructor stub

}

CFeatureList::~CFeatureList()
{
	// Deallocate the models
	for(int i = features.size() - 1; i > -1; i--)
		delete features[i];
}

void CFeatureList::Append(CFeature * feature)
{
	this->features.push_back(feature);
}

int CFeatureList::GetNFreeParameters()
{
	// TODO: we can use memoization to speed this up, avoid O(n) cost.

	int n = 0;
    for(vector<CFeature*>::iterator it = features.begin(); it != features.end(); ++it)
    {
    	n += (*it)->GetNFreeParameters();
    }

    return n;
}


// This function sets the parameters for models prior to a GetData call.
void CFeatureList::SetParams(double * params, int n_params)
{

}
