#include "CGLWidget.h"

#include "CMinimizer.h"
#include "CMinimizer_mpfit.h"


CGLWidget::CGLWidget(QWidget *parent, string shader_source_dir, string cl_kernel_dir)
    : QGLWidget(parent), mGLT(this, shader_source_dir, cl_kernel_dir), mMinThread()
{ 
    setAutoBufferSwap(false);
    this->doneCurrent();
}

CGLWidget::~CGLWidget()
{
	stopRendering();
}

void CGLWidget::AddModel(eModels model)
{
	mGLT.AddModel(model);
}

void CGLWidget::EnqueueOperation(CL_GLT_Operations op)
{
	mGLT.EnqueueOperation(op);
}

void CGLWidget::closeEvent(QCloseEvent *evt)
{
    stopRendering();
    QGLWidget::closeEvent(evt);
}

void CGLWidget::LoadMinimizer()
{
	// TODO: Permit loading of different minimizers here.
	CMinimizer * tmp;
	tmp = (CMinimizer*) new CMinimizer_mpfit(&mGLT);
	mMinThread.SetMinimizer(tmp);
}

void CGLWidget::paintEvent(QPaintEvent *)
{
    mGLT.EnqueueOperation(GLT_RenderModels);
}
void CGLWidget::resizeEvent(QResizeEvent *evt)
{
    mGLT.resizeViewport(evt->size());
}

void CGLWidget::RunMinimizer()
{
	mMinThread.start();
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

void CGLWidget::SetParameters(float * params, int n_params)
{
	mGLT.SetParameters(params, n_params);
}

void CGLWidget::SetScale(double scale)
{
	mGLT.SetScale(scale);
}

void CGLWidget::SetShader(int model_id, eGLShaders shader)
{
	mGLT.SetShader(model_id, shader);
}

void CGLWidget::SetPositionType(int model_id, ePositionTypes pos_type)
{
	mGLT.SetPositionType(model_id, pos_type);
}

