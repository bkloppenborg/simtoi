
#ifndef GLTHREAD
#define GLTHREAD

#include <QObject>
#include <QThread>
#include <QSize>
#include <QMutex>
#include <QSemaphore>
#include <string>
#include <queue>

class CGLWidget;

using namespace std;

// A list of operations permitted.
enum GLT_Operations
{
	GLT_BlitToScreen,
	GLT_Resize,
	GLT_Redraw,
	GLT_Stop
};

/// A quick class for making priority queue comparisions.  Used in CGLThread
class GLQueueComparision
{
public:
	GLQueueComparision() {};

	bool operator() (const GLT_Operations& lhs, const GLT_Operations&rhs) const
	{
		if(rhs == GLT_Stop)
			return true;

		return false;
	}
};


class CGLThread : public QThread {
    Q_OBJECT
private:
	priority_queue<GLT_Operations, vector<GLT_Operations>, GLQueueComparision> mQueue;
    QMutex mQueueMutex;
    QSemaphore mQueueSemaphore;
    CGLWidget * mGLWidget;
    bool mPermitResize;
    bool mResizeInProgress;
    int mWidth;
    int mHeight;
    double mScale;


    bool doRendering;
    bool doResize;
    float rotAngle;
    int id;
    static int count;

public:
    CGLThread(CGLWidget * glWidget);
    static void CheckOpenGLError(string function_name);

    void EnqueueOperation(GLT_Operations op);
    GLT_Operations GetNextOperation(void);

private:
    void glDrawTriangle();

public:
    void resizeViewport(const QSize &size);
    void resizeViewport(int width, int height);
    void run();
    void stop();

};
    
#endif
