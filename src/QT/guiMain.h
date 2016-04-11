 /*
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
 *
 * Copyright (c) 2012 Brian Kloppenborg
 */
 
#ifndef CGUIMAIN_H
#define CGUIMAIN_H

#include "ui_guiMain.h"

#include <QtGui/QMainWindow>
#include <QStandardItem>
#include <QStandardItemModel>
#include <string>
#include <memory>

using namespace std;

class CParameters;
class CParameterItem;
class CGLWidget;
class wAnimation;
class wMinimizer;

typedef shared_ptr<CGLWidget> CGLWidgetPtr;

class guiMain : public QMainWindow, private Ui::guiMain
{
    Q_OBJECT

protected:
	wAnimation * wAnimationWidget; ///< pointer to animiation widget. Re-parented once created, don't delete.
	wMinimizer * wMinimizerWidget; ///< pointer to the minimizer widget. Re-parented once created, don't delete.

    string mShaderSourceDir;
    string mKernelSourceDir;

    string mOpenSaveFileDir; 	// Stores the previously opened directory for models

public:
    guiMain(QWidget *parent = 0);
    virtual ~guiMain();

protected:
    void toggleWidgets();

protected:
    void Init();
public:
    void Open(QString & filename);
    void run_command_line(QStringList & data_files, QString & model_file, string minimizer_id, string save_directory, bool close_simtoi);

private slots:

	void displayWarning(string);

	void on_actionNew_triggered(void);
    void on_actionExport_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
};

#endif // CGUIMAIN_H
