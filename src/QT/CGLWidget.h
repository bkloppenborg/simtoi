
#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>

#include "CGLThread.h"

class CModel;

class CGLWidget : public QGLWidget {
    Q_OBJECT
    
protected:
    CGLThread mGLT;

public:
    CGLWidget(QWidget *parent, string shader_source_dir);

    void AddModel(eModels model, eGLShaders shader);
protected:
    void closeEvent(QCloseEvent *evt);
public:
    void EnqueueOperation(GLT_Operations op);
	vector< pair<eGLShaders, string> > GetShaderNames(void) { return mGLT.GetShaderNames(); };

protected:
	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:
    void SetScale(double scale);
    void startRendering();
    void stopRendering();
};

#endif
