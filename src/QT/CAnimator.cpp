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

CAnimator::CAnimator(QObject * parent)
	: QThread(parent)
{
	mRun = false;
	mTime = 0;
	mStep = 0;
}

CAnimator::~CAnimator()
{
	stop();
	wait();
}

void CAnimator::run()
{
	mRun = true;

	if(!mGLWidget)
		return;

	while(mRun)
	{
		mGLWidget->SetTime(mTime);
		mGLWidget->Render();

		this->msleep(50);
		mTime += mStep;

		emit(update_time(mTime));
	}
}

/// Sets the current widget. Connects necessary signals and slots.
void CAnimator::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;
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

