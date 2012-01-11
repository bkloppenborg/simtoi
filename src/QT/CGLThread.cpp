
#include <QTime>
#include <QtDebug>
#include <GL/gl.h>
#include <GL/glu.h>

#include "CGLThread.h"
#include "CGLWidget.h"

int CGLThread::count = 0;

CGLThread::CGLThread(CGLWidget *glWidget) : QThread(), mGLWidget(glWidget) {
    doRendering = true;
    doResize = false;
    id = count++;
    qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " Created";
}
    
void CGLThread::resizeViewport(const QSize &size){
    qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " resizeViewport";
    w = size.width();
    h = size.height();
    doResize = true;
}   

/// Static function for checking OpenGL errors:
void CGLThread::CheckOpenGLError(string function_name)
{
    GLenum status = glGetError(); // Check that status of our generated frame buffer
    // If the frame buffer does not report back as complete
    if (status != 0)
    {
        string errstr =  (const char *) gluErrorString(status);
        printf("Encountered OpenGL Error %x %s\n %s", status, errstr.c_str(), function_name.c_str());
        throw;
    }
}

void CGLThread::run(){
    qDebug() << id << ":run..";
    
    mGLWidget->makeCurrent();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix
    gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
    glMatrixMode(GL_MODELVIEW);

    while (doRendering) {
        rotAngle = rotAngle + (id+1)*3; // threads rotate pyramid at different rate!
        qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << ":rendering...";
        if (doResize) {
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
            glMatrixMode(GL_MODELVIEW);
            doResize = false;
        }
        // Rendering code goes here
        glDrawTriangle();
        mGLWidget->updateGL();
        msleep(40);
    }
}

void CGLThread::stop()   {
    qDebug() << "time=" << QTime::currentTime().msec() << " thread=" << id << " STOP";
    doRendering = false;
}
    
void CGLThread::glDrawTriangle() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View

    glTranslatef(-1.5f,0.0f,-6.0f);
  
    glRotatef(rotAngle,0.0f,1.0f,0.0f);		// Rotate The Pyramid On The Y axis 

    // draw a pyramid (in smooth coloring mode)
    glBegin(GL_POLYGON);				// start drawing a pyramid

    // front face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
    glVertex3f(0.0f, 1.0f, 0.0f);		        // Top of triangle (front)
    glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
    glVertex3f(-1.0f,-1.0f, 1.0f);		// left of triangle (front)
    glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
    glVertex3f(1.0f,-1.0f, 1.0f);		        // right of traingle (front)	

    // right face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Right)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f( 1.0f,-1.0f, 1.0f);		// Left Of Triangle (Right)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);		// Right Of Triangle (Right)

    // back face of pyramid
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Back)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);		// Left Of Triangle (Back)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f(-1.0f,-1.0f, -1.0f);		// Right Of Triangle (Back)

    // left face of pyramid.
    glColor3f(1.0f,0.0f,0.0f);			// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Left)
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f(-1.0f,-1.0f,-1.0f);		// Left Of Triangle (Left)
    glColor3f(0.0f,1.0f,0.0f);			// Green
    glVertex3f(-1.0f,-1.0f, 1.0f);		// Right Of Triangle (Left)

    glEnd();					// Done Drawing The Pyramid
}
