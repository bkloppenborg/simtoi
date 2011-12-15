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

#include "main.h"
#include "CUI.h"

using namespace std;

// The main routine.
int main(int argc, char *argv[])
{
	// Pass off to the GUI:
    QApplication a(argc, argv);
    CUI w;
    w.show();
    return a.exec();
}


#endif /* MAIN_CPP_ */
