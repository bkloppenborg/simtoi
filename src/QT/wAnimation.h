/*
 * wAnimation.h
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#ifndef WANIMATION_H_
#define WANIMATION_H_

#include <QWidget>
#include <QThread>
#include <memory>

#include "ui_wAnimation.h"
#include "CAnimator.h"

using namespace std;

class CWorkQueue;
typedef shared_ptr<CWorkQueue> CQueuePtr;

class wAnimation : public QWidget, public Ui::wAnimiation {
	Q_OBJECT

protected:
	CGLWidgetPtr mGLWidget;
    CAnimator mAnimator;

public:
    wAnimation(QWidget * parent = 0);
	virtual ~wAnimation();

	void setGLWidget(CGLWidgetPtr gl_widget);

	void toggleButtons();

protected:
	void enqueueRender(double time);

signals:
	void timestep_updated(double new_step_size);

public slots:

	void on_btnStepBackward2_clicked();
	void on_btnStepBackward_clicked();
	void on_btnPlayPause_clicked();
	void on_btnStepForward_clicked();
	void on_btnStepForward2_clicked();

	void on_doubleSpinBoxJD_valueChanged(double value);
	void on_doubleSpinBoxRate_valueChanged(double value);

	void update_time(double new_time);

};

#endif /* WANIMATION_H_ */
