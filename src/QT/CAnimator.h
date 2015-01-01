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

using namespace std;
class CGLWidget;
typedef shared_ptr<CGLWidget> CGLWidgetPtr;

using namespace std;

class CAnimator : public QThread
{
	Q_OBJECT
public:
	CGLWidgetPtr mGLWidget;

protected:
	atomic<bool> mRun;

	double mTime;
	double mStep;

public:
	CAnimator(CGLWidgetPtr gl_widget, QObject * parent = 0);
	~CAnimator();

	void stop();

	void run();

public slots:
	void setTime(double time);
	void setStep(double step);

signals:
	void update_time(double new_time);
};

#endif /* CANIMATOR_H_ */
