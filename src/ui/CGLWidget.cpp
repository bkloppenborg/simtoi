/*
 * CCGLWidget.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 */

#include "CGLWidget.h"
#include <QtGui/QMouseEvent>
#include <cstdio>
#include "COpenGL.h"

using namespace std;

CGLWidget::CGLWidget(QWidget *parent) : QGLWidget(parent)
{
	// Enable mouse tracking
    //setMouseTracking(true);
	setAutoBufferSwap(false);

	// Create the rendering thread, pass off the rendering operation, then start the thread.
	mGL = new COpenGL(this);
	// TODO: Pull these values from the GUI.
	mGL->Init(128, 128, 0.05, "/home/bkloppenborg/workspace/SIMTOI/src/shaders");

	mGLThread = new QThread();
	mGLThread->start();

	mGL->moveToThread(mGLThread);

	// Connect some signals and slots:
	connect(this, SIGNAL(initializeGL(void)), mGL, SLOT(InitOpenGL(void)));
	connect(this, SIGNAL(resizeGL(int, int)), mGL, SLOT(Resize(int, int)));
	connect(this, SIGNAL(paintGL(void)), mGL, SLOT(Redraw(void)));

	this->doneCurrent();
}
