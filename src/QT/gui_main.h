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
#include "gui_common.h"

#include <QtGui/QMainWindow>
#include <QStandardItem>
#include <QStandardItemModel>
#include <string>

using namespace std;

class CParameters;
class CParameterItem;

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

public:
    gui_main(QWidget *parent = 0);
    virtual ~gui_main();

protected:
    QMdiSubWindow * AddGLArea(int model_width, int model_height, double model_scale);

public:
    void AutoClose(bool auto_close, QMdiSubWindow * sw);

protected:
    void ButtonCheck();
    void close();
    void closeEvent(QCloseEvent *evt);

public:
    void CommandLine(QStringList & data_files, QStringList & model_files, int minimizer, int size, double scale, bool close_simtoi);

protected:
    void DataAdd(QStringList & filenames, QMdiSubWindow * sw);

    void MinimizerRun(string MinimizerID, QMdiSubWindow * sw);
    void ModelOpen(QStringList & fileNames, QMdiSubWindow * sw);

    void Init();

private slots:
    void Animation_StartStop();
    void Animation_Reset();
    void AutoClose(QWidget * widget);
    void DataAdd(void);
    void DataRemove();
    void DeleteGLArea();
    void ExportPhotometry();
    void ExportFITS();
    void render();
    void on_btnMinimizerStart_clicked();
    void on_btnMinimizerStop_clicked();
    void on_btnNewModelArea_clicked();
	void ModelAdd(void);
	void ModelDelete(void);
	void ModelEdit(void);
    void ModelOpen();
    void ModelSave();
    void SetSavePath();
    void SetTime();


public slots:
	void subwindowSelected(QMdiSubWindow * window);
};

#endif // CMAINGUI_H
