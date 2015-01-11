/*
 * wMinimizer.h
 *
 *  Created on: Jan 11, 2014
 *      Author: bkloppenborg
 */

#ifndef WMINIMIZER_H_
#define WMINIMIZER_H_

#include <QWidget>
#include <QThread>
#include <memory>

#include "ui_wMinimizer.h"
#include "CMinimizerThread.h"

using namespace std;

class CGLWidget;
typedef shared_ptr<CGLWidget> CGLWidgetPtr;

class CMinimizerThread;
typedef shared_ptr<CMinimizerThread> CMinimizerPtr;

class wMinimizer : public QWidget, public Ui::wMinimizer {
	Q_OBJECT

protected:
	CGLWidgetPtr mGLWidget;
	CMinimizerPtr mMinimizer;

public:
	wMinimizer(QWidget * parent = 0);
	virtual ~wMinimizer();

	void startMinimizer(const string & minimizer_id, const string & save_directory);

	void setGLWidget(CGLWidgetPtr gl_widget);
protected:
	void setSaveDirectory(const string & save_directory);

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
