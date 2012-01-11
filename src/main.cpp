/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <QtGui>
#include <QApplication>
#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

#include "main.h"
#include "cmaingui.h"

using namespace std;

// The main routine.
int main(int argc, char *argv[])
{
	// Pass off to the GUI:
    QApplication app(argc, argv);
#ifdef Q_WS_X11
    XInitThreads();
    // For QT 4.8 we'll need to use this line instead.
    //app.setAttribute(Qt::AA_X11InitThreads, true);
#endif
    cmaingui main_window;
    main_window.show();
    return app.exec();
}


#endif /* MAIN_CPP_ */
