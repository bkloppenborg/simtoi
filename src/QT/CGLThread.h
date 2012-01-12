
#ifndef GLTHREAD
#define GLTHREAD

#include <QObject>
#include <QThread>
#include <QSize>

class CGLWidget;

class CGLThread : public QThread {
    Q_OBJECT
public:
    CGLThread(CGLWidget * glWidget);
    void resizeViewport(const QSize &size);  
    void run();
    void stop();
private:
    void glDrawTriangle();
    bool doRendering;
    bool doResize;
    int w;
    int h;
    float rotAngle;
    CGLWidget * mGLWidget;
    int id;
    static int count;
};
    
#endif
