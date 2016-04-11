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

#ifndef WMINIMIZER_H_
#define WMINIMIZER_H_

#include <QWidget>
#include <QThread>
#include <memory>

#include "ui_wMinimizer.h"
#include "CMinimizerThread.h"
#include "CGLWidget.h"

using namespace std;

class CMinimizerThread;
typedef shared_ptr<CMinimizerThread> CMinimizerPtr;

class wMinimizer : public QWidget, public Ui::wMinimizer {
	Q_OBJECT

protected:
	CGLWidget * mGLWidget;
	CMinimizerPtr mMinimizer;
	string mSaveDirectory;

public:
	wMinimizer(QWidget * parent = 0);
	virtual ~wMinimizer();

	void startMinimizer(const string & minimizer_id, const string & save_directory);

	void setGLWidget(CGLWidget * gl_widget);
protected:
	void setSaveDirectory(const string & save_directory);

public:
	void toggleButtons();

signals:
	void finished();
	void started();

public slots:
	void on_btnStartStop_clicked();

protected slots:
	void minimizerFinished();
	void minimizerStarted();

};

#endif /* WMINIMIZER_H_ */
