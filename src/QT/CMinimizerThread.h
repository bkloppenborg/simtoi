/*
 * CMinimizerThread.h
 *
 *  Created on: Jan 30, 2012
 *      Author: bkloppenborg
 */

#ifndef CMINIMIZERTHREAD_H_
#define CMINIMIZERTHREAD_H_

#include <QObject>
#include <QThread>

class CMinimizer;

class CMinimizerThread: public QThread
{
    Q_OBJECT

protected:
    CMinimizer * mMinimizer;

public:
	CMinimizerThread();
	virtual ~CMinimizerThread();

	void SetMinimizer(CMinimizer * minimizer);

	void run();
	void stop();

	signals:
	void ParametersChanged();
};

#endif /* CMINIMIZERTHREAD_H_ */
