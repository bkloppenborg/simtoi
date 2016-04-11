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

#ifndef CANIMATOR_H_
#define CANIMATOR_H_

#include <QObject>
#include <QThread>
#include <memory>
#include <atomic>
#include "CGLWidget.h"

using namespace std;

class CAnimator : public QThread
{
	Q_OBJECT
public:
	CGLWidget * mGLWidget;

protected:
	atomic<bool> mRun;

	double mTime;
	double mStep;

public:
	CAnimator(QObject * parent = 0);
	~CAnimator();

	void setGLWidget(CGLWidget * gl_widget);
	void stop();

	void run();

signals:
	void updateTime(double new_time);

public slots:
	void setTime(double time);
	void setStep(double step);
};

#endif /* CANIMATOR_H_ */
