
#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>

#include "CCL_GLThread.h"
#include "CLibOI.h"

class CModel;

class CGLWidget : public QGLWidget
{
    Q_OBJECT
    
protected:
    CCL_GLThread mGLT;

public:
    CGLWidget(QWidget *parent, string shader_source_dir, string cl_kernel_dir);
    ~CGLWidget();

    void AddModel(eModels model);
protected:
    void closeEvent(QCloseEvent *evt);
public:

    void EnqueueOperation(CL_GLT_Operations op);
	vector< pair<eGLShaders, string> > GetShaderNames(void) { return mGLT.GetShaderNames(); };

protected:
	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:
    CModelList * GetModelList() { return mGLT.GetModelList(); };

    void LoadData(string filename) { mGLT.LoadData(filename); };

    bool OpenCLInitialized() { return mGLT.OpenCLInitialized(); };

    void SetParameters(float * params, int n_params);
    void SetScale(double scale);
    void SetShader(int model_id, eGLShaders shader);
    void SetPositionType(int model_id, ePositionTypes pos_type);
    void SetTime(double t) { mGLT.SetTime(t); };
    void SetTimestep(double dt) { mGLT.SetTimestep(dt); };

    void startRendering();
    void stopRendering();
};

#endif
