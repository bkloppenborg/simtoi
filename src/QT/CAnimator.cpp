/*
 * CAnimator.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: bkloppenborg
 */

#include <QThread>
#include <iostream>
using namespace std;

#include "CAnimator.h"
#include "CGLWidget.h"

CAnimator::CAnimator(CGLWidgetPtr gl_widget, QObject * parent)
	: QThread(parent)
{
	mRun = false;
	mTime = 0;
	mStep = 0;

	mGLWidget = gl_widget;
}

CAnimator::~CAnimator()
{
	stop();
	wait();
}

void CAnimator::run()
{
	mRun = true;

	while(mRun)
	{
		mGLWidget->SetTime(mTime);
		mGLWidget->Render();

		this->msleep(50);
		mTime += mStep;

		emit(update_time(mTime));
	}
}

void CAnimator::setTime(double time)
{
	mTime = time;
}

void CAnimator::setStep(double step)
{
	mStep = step;
}

void CAnimator::stop()
{
	mRun = false;
}

