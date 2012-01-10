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
#include "CUI.h"

using namespace std;

// The main routine.
int main(int argc, char *argv[])
{
#ifdef Q_WS_X11
    XInitThreads();
#endif
	// Pass off to the GUI:
    QApplication a(argc, argv);
    // For QT 4.8 we'll need to enable this line:
    //a.setAttribute(Qt::AA_X11InitThreads, true);
    CUI w;
    w.show();
    return a.exec();
}


#endif /* MAIN_CPP_ */
