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

typedef shared_ptr<CGLWidget> CGLWidgetPtr;

class gui_main : public QMainWindow, private Ui::cmainguiClass
{
    Q_OBJECT

    string mShaderSourceDir;
    string mKernelSourceDir;
    bool mAnimating;
    bool mAutoClose;

    string mDefaultSaveDir; // Stores the default save path
    int mNumMinimizations; // The number of minimization runs (used for automatic savefile naming).
    string mOpenDataDir;	// Stores the previously opened directory for data files
    string mOpenModelDir; 	// Stores the previously opened directory for models

    CGLWidgetPtr mGLWidget;

public:
    gui_main(QWidget *parent = 0);
    virtual ~gui_main();

protected:
    void AddGLArea(CGLWidgetPtr widget);
    void AddData(QStringList & filenames);
public:
    void AutoClose(bool auto_close);

protected:
    void ButtonCheck();

    void close();
    void closeEvent(QCloseEvent *evt);
public:
    void CommandLine(QStringList & data_files, QString & model_file, string minimizer, bool close_simtoi);

protected:
    void MinimizerRun(string MinimizerID);
    void ModelOpen(QStringList & fileNames);

public:
    void Open(QString & filename);

protected:
    void Init();

public:
    void render_at_time(double time);

public:
    void SetOutputDir(string folder_name);
    void TreeCheck();

private slots:
    void minimizerFinished();

    void on_doubleSpinBoxJD_valueChanged(double jd);

    void on_btnAddModel_clicked();
    void on_btnEditModel_clicked();
    void on_btnRemoveModel_clicked();

    void on_actionExport_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_btnAddData_clicked();

    void on_btnRemoveData_clicked();
    void on_btnMinimizerStartStop_clicked();
    void on_btnNewModelArea_clicked();

    void on_btnPlayPause_clicked();
    void on_btnStepBackward_clicked();
    void on_btnStepBackward2_clicked();
    void on_btnStepForward_clicked();
    void on_btnStepForward2_clicked();

};

#endif // CMAINGUI_H
