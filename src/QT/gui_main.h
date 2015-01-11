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
 
#ifndef CMAINGUI_H
#define CMAINGUI_H

#include "ui_gui_main.h"

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

class gui_main : public QMainWindow, private Ui::cmainguiClass
{
    Q_OBJECT

protected:
    CGLWidgetPtr mGLWidget;

	wAnimation * wAnimationWidget; // pointer to animiation widget. Re-parented once created, don't delete.
	wMinimizer * wMinimizerWidget; // pointer to the minimizer widget. Re-parented once created, don't delete.

    string mShaderSourceDir;
    string mKernelSourceDir;
    bool mAnimating;
    bool mAutoClose;

    string mDefaultSaveDir; // Stores the default save path
    int mNumMinimizations; // The number of minimization runs (used for automatic savefile naming).
    string mOpenDataDir;	// Stores the previously opened directory for data files
    string mOpenModelDir; 	// Stores the previously opened directory for models

public:
    gui_main(QWidget *parent = 0);
    virtual ~gui_main();

protected:
    void AddGLArea(CGLWidgetPtr widget);

protected:
    void Init();
public:
    void Open(QString & filename);
    void run_command_line(QStringList & data_files, QString & model_file, string minimizer_id, string save_directory, bool close_simtoi);


private slots:
	void on_actionNew_triggered(void);
    void on_actionExport_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
};

#endif // CMAINGUI_H
