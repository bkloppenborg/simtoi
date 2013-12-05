/*
 * CAnimator.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: bkloppenborg
 */

#include <QThread>

#include "CAnimator.h"
#include "CGLWidget.h"

CAnimator::CAnimator(CGLWidget * parent)
{
	mTime = 0;
	mRun = false;
	mParent = parent;
}

CAnimator::~CAnimator()
{
	mRun = false;
}

void CAnimator::start_animation(double start, double step)
{
	mTime = start;
	mRun = true;

	while(mRun)
	{
		mParent->SetTime(mTime);
		mParent->Render();

		Sleeper::msleep(50);
		mTime += step;
	}

	cout << "Animation stopped." << endl;
}

void CAnimator::stop_animation()
{
	mRun = false;
}
