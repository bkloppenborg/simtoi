/*
 * COI.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 */

#include "COI.h"

COI::COI(CCL_GLThread * WorkerThread)
	: CWorker(WorkerThread)
{

}

COI::~COI()
{
	// TODO Auto-generated destructor stub
}

string COI::GetDataDescription()
{
	return "OIFITS Data";
}

vector<string> COI::GetDataTypes()
{
	vector<string> temp;
	temp.push_back("*.oifits");
	temp.push_back("*.fits");
	return temp;
}

unsigned int COI::GetNData()
{
	return 0;
}

void COI::GetResiduals(valarray<double> & residuals)
{

}
void COI::GetUncertainties(valarray<double> & uncertainties)
{

}

void COI::LoadData(string filename)
{

}
