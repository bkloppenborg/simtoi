/*
 * CAnimator.h
 *
 *  Created on: Dec 4, 2013
 *      Author: bkloppenborg
 */

#ifndef CANIMATOR_H_
#define CANIMATOR_H_

#include <QObject>
#include <QThread>

class CGLWidget;

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

class CAnimator : public QObject
{
	Q_OBJECT

public:
	CGLWidget * mParent;

	double mTime;
	bool mRun;

public:
	CAnimator(CGLWidget * parent);
	~CAnimator();

	bool IsRunning() { return mRun; };

public slots:
	void start_animation(double start, double step);
	void stop_animation();
};

#endif /* CANIMATOR_H_ */
