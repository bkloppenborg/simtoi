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

	string mDefaultSaveDir;

public:
	wMinimizer(QWidget * parent = 0);
	virtual ~wMinimizer();

	void setGLWidget(CGLWidgetPtr gl_widget);

	void toggleButtons();


signals:
	void minimization_complete();
};

#endif /* WMINIMIZER_H_ */
