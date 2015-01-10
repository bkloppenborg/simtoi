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

void wAnimation::enqueueRender(double time)
{
	mGLWidget->SetTime(time);
	mGLWidget->Render();

	update_time(time);
}

void wAnimation::on_btnStepBackward2_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time - 10 *step);
}

void wAnimation::on_btnStepBackward_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time - step);
}

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

void wAnimation::on_btnStepForward_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time + step);
}

void wAnimation::on_btnStepForward2_clicked()
{
	double start_time = this->doubleSpinBoxJD->value();
	double step = this->doubleSpinBoxRate->value();

	enqueueRender(start_time + 10 * step);
}

void wAnimation::on_doubleSpinBoxJD_valueChanged(double value)
{
	double start_time = this->doubleSpinBoxJD->value();

	if(!mAnimator.isRunning())
		enqueueRender(start_time);
}

void wAnimation::on_doubleSpinBoxRate_valueChanged(double value)
{
	emit(timestep_updated(value));
}

/// Sets the current widget. Connects necessary signals and slots.
void wAnimation::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

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

void wAnimation::update_time(double value)
{
	this->doubleSpinBoxJD->setValue(value);
}
