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

#include <QtGui/QMainWindow>
#include <string>
#include <QStandardItem>
#include <QStandardItemModel>
#include "ui_gui_main.h"

class CParameters;
class CParameterItem;

using namespace std;

class gui_main : public QMainWindow
{
    Q_OBJECT

protected:
    Ui::cmainguiClass ui;
    string mShaderSourceDir;
    string mKernelSourceDir;
    bool mAnimating;

    string mOpenDataDir;	// Stores the previously opened directory for data files
    string mOpenModelDir; 	// Stores the previously opened directory for models

public:
    gui_main(QWidget *parent = 0);
    ~gui_main();

protected:
    void close();
    void closeEvent(QCloseEvent *evt);
    void ButtonCheck();
    void SetupComboBoxes();

private slots:
    void AddGLArea();
    void AddData(void);
    void Animation_StartStop();
    void Animation_Reset();
    void DeleteGLArea();
    void ExportPhotometry();
    void ExportFITS();
    void render();
	void ModelAdd(void);
	void ModelDelete(void);
	void ModelEdit(void);
    void open();
    void RunMinimizer();
    void RemoveData();
    void save();
    void SetTime();
    void StopMinimizer();


public slots:
	void subwindowSelected(QMdiSubWindow * window);
};

#endif // CMAINGUI_H
