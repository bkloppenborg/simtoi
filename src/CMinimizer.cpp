/*
 * CMinimizer.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: bkloppenborg
 */

#include "CMinimizer.h"
//#include "CMinimizer_MultiNest.h"

CMinimizer::CMinimizer(CSIMTOI * simtoi)
{
	mSIMTOI = simtoi;
}

CMinimizer::~CMinimizer()
{

}

CMinimizer * CMinimizer::GetMinimizer(string name, CSIMTOI * simtoi)
{
	CMinimizer * tmp = NULL;

	// Just match the string
//	if(name == "MultiNest")
//		tmp = new CMinimizer_MultiNest(simtoi);

	return tmp;
}

void CMinimizer::Run()
{
	//mThread = new boost::thread(boost::bind(&CMinimizer::ThreadFunc, this));
}

void CMinimizer::Stop()
{

}
