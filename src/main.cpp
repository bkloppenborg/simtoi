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
#include <QGLFormat>

// Include to enable threads on X11
#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif // Q_WS_X11

// Definition of the QT version. These should be set by the CMake script using compiler directives.
#ifndef QT_VERSION_MAJOR
#define QT_VERSION_MAJOR 4
#endif // QT_VERSION_MAJOR
#ifndef QT_VERSION_MINOR
#define QT_VERSION_MINOR 0
#endif // QT_VERSION_MINOR

#include <iostream>

#include "main.h"
#include "QT/guiMain.h"
#include "minimizers/load_minimizers.h"
#include "models/load_models.h"
#include "features/load_features.h"
#include "positions/load_positions.h"

using namespace std;

extern string EXE_FOLDER;

// The main routine.
int main(int argc, char *argv[])
{
	// X11 requires some special initialzation for threaded OpenGL rendering.
#if QT_VERSION_MAJOR >= 4 && QT_VERSION_MINOR >= 8
	// For QT >= 4.8, we have to set a global QT attribute
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
#elif defined Q_WS_X11
    // For all other versions of QT running on X11, the following (should) be sufficient.
    XInitThreads();
#endif

	// Pass off to the GUI:
    QApplication app(argc, argv);

	// Set the default OpenGL format
	QGLFormat fmt;
	fmt.setVersion(3, 2);
	fmt.setProfile(QGLFormat::CoreProfile);
	fmt.setDoubleBuffer(true);
	fmt.setSampleBuffers(true);
	fmt.setSamples(4);
	QGLFormat::setDefaultFormat(fmt);


    // determine the absolute directory from which SIMTOI is running
    EXE_FOLDER = app.applicationDirPath().toStdString();

    // Setup minimization engines:
    minimizers::load();
    models::load();
    features::load();
    positions::load();

    // get the list of command line arguments and parse them.
    QStringList args = app.arguments();
    QStringList data_files;
    QString model_file;
    string minimizer_id = "";
    string save_directory = "/tmp/model";
    bool close_simtoi = false;

    // If there were command-line options, parse them
    bool run_simtoi = false;
    if(args.size() > 0)
    	run_simtoi = ParseArgs(args, data_files, model_file, minimizer_id, save_directory, close_simtoi);

    if(run_simtoi)
    {
		// Startup the GUI:
		guiMain main_window;
		main_window.show();

		if(data_files.size() > 0 || model_file.size() > 0)
			main_window.run_command_line(data_files, model_file, minimizer_id, save_directory, close_simtoi);

		return app.exec();
    }

    return 0;
}

/// Parse the command line arguments splitting them into data files, model files, minimizer names, model area size and model area scale
bool ParseArgs(QStringList args, QStringList & filenames, QString & model_file, string &  minimizer, string & output_dir, bool & close_simtoi)
{
	unsigned int n_items = args.size();

	string value;
	QDir tmp = QDir(".");
	bool run_simtoi = true;

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
			model_file =tmp.absoluteFilePath(args.at(i + 1));

		if(value == "-o")
			output_dir = tmp.absoluteFilePath(args.at(i + 1)).toStdString();

		if(value == "--list-engines")
		{
			run_simtoi = false;

			vector<string> ids = CMinimizerFactory::getInstance().getIDs();
			vector<string> names = CMinimizerFactory::getInstance().getNames();
			printFactoryDescription(ids, names, "Engine ID");
		}

		if(value == "--list-models")
		{
			run_simtoi = false;

			vector<string> ids = CModelFactory::getInstance().getIDs();
			vector<string> names = CModelFactory::getInstance().getNames();
			printFactoryDescription(ids, names, "Model ID");
		}

		if(value == "--list-features")
		{
			run_simtoi = false;

			vector<string> ids = CFeatureFactory::getInstance().getIDs();
			vector<string> names = CFeatureFactory::getInstance().getNames();
			printFactoryDescription(ids, names, "Feature ID");
		}
	}

	return run_simtoi;
}

void printFactoryDescription(const vector<string> & ids, const vector<string> & names, const string & title)
{
	int field_width = 25;
	cout << " " << std::left << std::setw(field_width + 1) << std::setfill(' ') << title << "Description" << endl;
	cout << " " << string(field_width, '-') << " " << string(field_width, '-') << endl;
	// print out a header
	for(int i = 0; i < ids.size(); i++)
	{
		cout << " " << std::left << std::setw(field_width) << std::setfill(' ') << ids[i] << " " << names[i] << endl;
	}
}

/// Prints the command line options and exits.
void PrintHelp()
{
	cout << "SIMTOI: The SImulation and Modeling Tool for Optical Interferometry" << endl;
	cout << "Copyright (c) Brian Kloppenborg and Fabien Baron 2012" << endl;
	cout << endl;
	cout << "Command line usage: simtoi [...]" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "  " << "-h, --help      : " << "Show this help message and exit" << endl;
	cout << "  " << "-c              : " << "Close SIMTOI after minimization completes [default: off]" << endl;
	cout << "  " << "-d              : " << "Input data file. Specify multiple -d to include " << endl;
	cout << "  " << "                  " << "many data files." << endl;
	cout << "  " << "-e              : " << "Minimization engine ID (see Wiki)" << endl;
	cout << "  " << "-m              : " << "Model input file" << endl;
	cout << "  " << "-o              : " << "Output directory" << endl;
	cout << "  " << "--list-engines  : " << "Lists all registered minimization engines" << endl;
	cout << "  " << "--list-models   : " << "Lists all registered models" << endl;
	cout << "  " << "--list-features : " << "Lists all registered features" << endl;
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
