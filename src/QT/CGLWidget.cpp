#include "CGLWidget.h"

CGLWidget::CGLWidget(QWidget *parent, string shader_source_dir)
    : QGLWidget(parent), mGLT(this, shader_source_dir)
{ 
    setAutoBufferSwap(false);
    this->doneCurrent();
}

void CGLWidget::AddModel(eModels model)
{
	mGLT.AddModel(model);
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

