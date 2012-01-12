#include "CGLWidget.h"

CGLWidget::CGLWidget(QWidget *parent)
    : QGLWidget(parent), mGLT(this)
{ 
    setAutoBufferSwap(false);
    this->doneCurrent();
}

void CGLWidget::EnqueueOperation(GLT_Operations op)
{
	mGLT.EnqueueOperation(op);
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
    stopRendering();
    QGLWidget::closeEvent(evt);
}
void CGLWidget::paintEvent(QPaintEvent *)
{
    // Handled by the GLThread.
}
void CGLWidget::resizeEvent(QResizeEvent *evt)
{
    mGLT.resizeViewport(evt->size());
}


void CGLWidget::startRendering()
{
    mGLT.start();
}

void CGLWidget::stopRendering()
{
    mGLT.stop();
    mGLT.wait();
}

void CGLWidget::SetScale(double scale)
{
	mGLT.SetScale(scale);
}

