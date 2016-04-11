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

#ifndef WANIMATION_H_
#define WANIMATION_H_

#include <QWidget>
#include <QThread>
#include <memory>

#include "ui_wAnimation.h"
#include "CAnimator.h"
#include "CGLWidget.h"

using namespace std;

class CWorkQueue;
typedef shared_ptr<CWorkQueue> CQueuePtr;

class wAnimation : public QWidget, public Ui::wAnimiation {
	Q_OBJECT

protected:
	CGLWidget * mGLWidget;
    CAnimator mAnimator;

    double mWavelengthSliderStep;

public:
    wAnimation(QWidget * parent = 0);
	virtual ~wAnimation();

	void setGLWidget(CGLWidget * gl_widget);

	void toggleButtons();

protected:
	void enqueueRender(double time);
	void setWavelength(double value);

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
	void on_doubleSpinBoxWavelength_valueChanged(double value);

	void on_slideWavelength_valueChanged(int value);

	void updateTime(double new_time);
	void updateWavelength(double new_wavelength);
};

#endif /* WANIMATION_H_ */
