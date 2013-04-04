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

#include <iostream>

#include "main.h"
#include "gui_main.h"

using namespace std;

extern string EXE_FOLDER;

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

    // determine the absolute directory from which SIMTOI is running
    EXE_FOLDER = app.applicationDirPath().toStdString();

    // get the list of command line arguments and parse them.
    QStringList args = app.arguments();
    QStringList data_files;
    QStringList model_files;
    string minimizer = "";
    bool close_simtoi = false;

    // If there were command-line options, parse them
    if(args.size() > 0)
    	ParseArgs(args, data_files, model_files, minimizer, close_simtoi);

    // Startup the GUI:
    gui_main main_window;
    main_window.show();

    if(data_files.size() > 0)
    	main_window.CommandLine(data_files, model_files, minimizer, close_simtoi);


    return app.exec();
}

/// Parse the command line arguments splitting them into data files, model files, minimizer names, model area size and model area scale
void ParseArgs(QStringList args, QStringList & filenames, QStringList & models, string &  minimizer, bool & close_simtoi)
{
	unsigned int n_items = args.size();

	string value;
	QDir tmp = QDir(".");

	for(int i = 0; i < n_items; i++)
	{
		value = args.at(i).toStdString();

		if(value == "-c")
			close_simtoi = true;

		// data file(s)
		if(value == "-d")
			filenames.append(tmp.absoluteFilePath(args.at(i + 1)));

		// minimization engine
		if(value == "-e")
			minimizer = args.at(i + 1).toStdString();

		if(value == "-h" || value == "-help")
			PrintHelp();

		// model file
		if(value == "-m")
			models.append(tmp.absoluteFilePath(args.at(i + 1)));

//		if(value == "-o")
//			savefile.append(tmp.absoluteFilePath(args.at(i + 1)));
	}
}

/// Prints the command line options and exits.
void PrintHelp()
{
	cout << "SIMTOI: The SImulation and Modeling Tool for Optical Interferometry" << endl;
	cout << "Command line usage: simtoi [...]" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "  " << "-h, --help   : " << "Show this help message and exit" << endl;
	cout << "  " << "-c           : " << "Close SIMTOI after minimization completes [default: off]" << endl;
	cout << "  " << "-d           : " << "Input OIFITS data file. Specify multiple -d to include " << endl;
	cout << "  " << "               " << "many data files." << endl;
	cout << "  " << "-e           : " << "Minimization engine ID (see Wiki or CMinimizer.h)" << endl;
	cout << "  " << "-m           : " << "Model input file" << endl;
	cout << endl;
	cout << "SIMTOI also supports QT commands. For instance you can run SIMTOI from a: " << endl;
	cout << "remotely executed script (or from gnu screen) by adding: " << endl;
	cout << "  " << "-display x:y : " << "Run SIMTOI on display 'y' of the computer named 'x'" << endl;
	cout << "In all cases, SIMTOI must be executed on a valid display on a computer with a" << endl;
	cout << "with a video card which supports OpenCL, OpenGL, and OpenCL-OpenGL interop." << endl;
	cout << endl;
	exit(0);
}

#endif /* MAIN_CPP_ */
