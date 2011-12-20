/*
 * CCGLWidget.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 */

#include "CGLWidget.h"
#include <QtGui/QMouseEvent>
#include <cstdio>
#include "COpenGLThread.h"

using namespace std;

CGLWidget::CGLWidget(QWidget *parent) : QGLWidget(parent)
{
	// Enable mouse tracking
    //setMouseTracking(true);

	// Create the rendering thread, pass off the rendering operation, then start the thread.
	mThread = new COpenGLThread(this);

	// Connect some signals and slots:
	connect(this, SIGNAL(initializeGL(void)), mThread, SLOT(InitOpenGL(void)));
	connect(this, SIGNAL(resizeGL(int, int)), mThread, SLOT(Resize(int, int)));
	connect(this, SIGNAL(paintGL(void)), mThread, SLOT(Redraw(void)));


	this->doneCurrent();
	mThread->start();
}
