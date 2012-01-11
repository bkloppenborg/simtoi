#include "CGLWidget.h"

CGLWidget::CGLWidget(QWidget *parent)
    : QGLWidget(parent), mGLT(this)
{ 
    setAutoBufferSwap(true);
    resize(320, 240);
    this->doneCurrent();
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
    stopRendering();
    QGLWidget::closeEvent(evt);
}


void CGLWidget::paintEvent(QPaintEvent *)
{
    // Handled by the mGLT (CGLThread).
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


