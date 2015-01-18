/*
 * CAnimator.h
 *
 *  Created on: Aug. 26, 2014
 *      Author: bkloppenborg
 *
 *  A simple thread for animating SIMTOI models.
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
