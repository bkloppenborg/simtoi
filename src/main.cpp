/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <QtGui>
#include <QApplication>
#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

#include "main.h"
#include "gui_main.h"

using namespace std;

// The main routine.
int main(int argc, char *argv[])
{
#ifdef Q_WS_X11
    // X11 requires an explicit initialization for threading:
    XInitThreads();
    // For QT 4.8 we'll need to use this line instead.
    //app.setAttribute(Qt::AA_X11InitThreads, true);
#endif

	// Pass off to the GUI:
    QApplication app(argc, argv);

    // get the list of command line arguments and parse them.
    QStringList args = app.arguments();
    QStringList data_files;
    QStringList model_files;
    int minimizer = 0;
    int width = 0;
    double scale = 0;
    bool close_simtoi = false;

    // Startup the GUI:
    gui_main main_window;
    main_window.show();

    // If there were command-line options, parse them
    if(args.size() > 0)
    	ParseArgs(args, data_files, model_files, minimizer, width, scale, close_simtoi);

    if(width > 0 && scale > 0)
    	main_window.CommandLine(data_files, model_files, minimizer, width, scale, close_simtoi);


    return app.exec();
}

/// Parse the command line arguments splitting them into data files, model files, minimizer names, model area size and model area scale
void ParseArgs(QStringList args, QStringList & filenames, QStringList & models, int &  minimizer, int & size, double & scale, bool & close_simtoi)
{
	unsigned int n_items = args.size();

	string value;
	QDir tmp = QDir(".");

	for(int i = 0; i < n_items; i++)
	{
		value = args.at(i).toStdString();

		// data file(s)
		if(value == "-d")
			filenames.append(tmp.absoluteFilePath(args.at(i + 1)));

		// model file
		if(value == "-m")
			models.append(tmp.absoluteFilePath(args.at(i + 1)));

		// minimization engine
		if(value == "-e")
			minimizer = args.at(i + 1).toInt();

		// model area width
		if(value == "-w")
			size = args.at(i+1).toInt();

		// model area scale
		if(value == "-s")
			scale = args.at(i+1).toDouble();

		if(value == "-c")
			close_simtoi = true;

		if(value == "-h")
			PrintHelp();
	}
}

void PrintHelp()
{
	printf("For command-line arguments see the wiki: \n https://github.com/bkloppenborg/simtoi/wiki/Command-Line \n");
	exit(0);
}

#endif /* MAIN_CPP_ */
