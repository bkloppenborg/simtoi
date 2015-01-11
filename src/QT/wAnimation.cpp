/*
 * wAnimation.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wAnimation.h"

#include "CAnimator.h"
#include "CGLWidget.h"

//class CWorkQueue;
//typedef shared_ptr<CWorkQueue> CQueuePtr;

wAnimation::wAnimation(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	// connect any non-automatic signal/slots
	connect(&mAnimator, SIGNAL(update_time(double)), this, SLOT(update_time(double)));
	connect(this, SIGNAL(timestep_updated(double)), &mAnimator, SLOT(setStep(double)));

	setEnabled(false);
}

wAnimation::~wAnimation()
{
	mAnimator.stop();
	mAnimator.wait();
}

/// Enqueues a render operation in the CGLWidget
void wAnimation::enqueueRender(double time)
{
	mGLWidget->SetTime(time);
	mGLWidget->Render();

	update_time(time);
}

/// Decrement the current time by 10 times the spinbox value, then enqueue a render
void wAnimation::on_btnStepBackward2_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time - 10 *step);
}

/// Decrement the time by spinbox value, then enqueue a render
void wAnimation::on_btnStepBackward_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time - step);
}

/// Start / stop the animiation widget, update the play/pause button
void wAnimation::on_btnPlayPause_clicked()
{
	if(mAnimator.isRunning())
	{
		mAnimator.stop();
		mAnimator.wait();

		btnPlayPause->setText(QString("P"));
	}
	else
	{
		double start_time = this->doubleSpinBoxJD->value();
		double step = this->doubleSpinBoxRate->value();

		mAnimator.setTime(start_time);
		mAnimator.setStep(step);
		mAnimator.start();

		btnPlayPause->setText(QString("||"));
	}
}

/// Increment the time by spinbox value, then enqueue a render
void wAnimation::on_btnStepForward_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time + step);
}

/// Increment the time by 10 times the spinbox value, then enqueue a render
void wAnimation::on_btnStepForward2_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time + 10 * step);
}

/// Update the current time to the value specified. Will do nothing if the
/// animation widget is running.
void wAnimation::on_doubleSpinBoxJD_valueChanged(double value)
{
	double start_time = this->doubleSpinBoxJD->value();

	if(!mAnimator.isRunning())
		enqueueRender(start_time);
}

/// Update the current timestep
void wAnimation::on_doubleSpinBoxRate_valueChanged(double value)
{
	emit(timestep_updated(value));
}

/// Sets the current widget. Connects necessary signals and slots.
void wAnimation::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

	mAnimator.stop();
	mAnimator.setGLWidget(gl_widget);

	toggleButtons();
}

/// Automatically (de)acitvate buttons depending on the status of various properties
/// of this widget.
void wAnimation::toggleButtons()
{
	this->setEnabled(false);

	if(mGLWidget)
		this->setEnabled(true);
}

/// Updates the current time stored in the animation widget.
void wAnimation::update_time(double value)
{
	this->doubleSpinBoxJD->setValue(value);
}
