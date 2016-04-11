 /*
 * This file is part of the SImulation and Modeling Tool for Optical
 * Interferometry (SIMTOI).
 *
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation version 3.
 *
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2014 Brian Kloppenborg
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

	mGLWidget = NULL;
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

		emit(updateTime(mTime));
	}
}

/// Sets the current widget. Connects necessary signals and slots.
void CAnimator::setGLWidget(CGLWidget * gl_widget)
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

