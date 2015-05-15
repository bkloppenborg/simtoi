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
 
#include "guiMain.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QTreeView>
#include <QStringList>
#include <QFileDialog>
#include <vector>
#include <utility>
#include <fstream>
#include <cmath>
#include <stdexcept>

// QT or GUI elements
#include "wAnimation.h"
#include "wMinimizer.h"
#include "wParameterEditor.h"
#include "CGLWidget.h"
#include "guiCommon.h"
#include "guiRegion.h"
#include "CTreeModel.h"
#include "CMinimizerThread.h"

#include "CPosition.h"

#include "CMinimizerFactory.h"


guiMain::guiMain(QWidget *parent_widget)
    : QMainWindow(parent_widget)
{
	Init();

    // Create a new animation widget, init the tab region
    wAnimationWidget = new wAnimation();
	this->tabBottom->addTab(wAnimationWidget, QString("Animation"));

    wMinimizerWidget = new wMinimizer();
	this->tabBottom->addTab(wMinimizerWidget, QString("Minimization"));

    this->wModelParameterEditor->setGLWidget(wGLWidget);
    this->wOpenDataEditor->setGLWidget(wGLWidget);
    this->wAnimationWidget->setGLWidget(wGLWidget);
    this->wMinimizerWidget->setGLWidget(wGLWidget);

	// establish cross-widget signal slot connections
	// minimization finished -> update parameter values
	connect(wMinimizerWidget, SIGNAL(finished()), wModelParameterEditor, SLOT(updateModels()));
	// data widget data selection ->
	connect(wOpenDataEditor, SIGNAL(timeSelected(double)), wAnimationWidget, SLOT(updateTime(double)));
	connect(wOpenDataEditor, SIGNAL(wavelengthSelected(double)), wAnimationWidget, SLOT(updateWavelength(double)));
	// data removal
	connect(wGLWidget, SIGNAL(dataRemoved(int)), wOpenDataEditor, SLOT(dataRemoved(int)));
}

guiMain::~guiMain()
{

}

/// Runs initialization routines for the main this->
void guiMain::Init(void)
{
	// Init the UI
	this->setupUi(this);
	this->setWindowTitle("SIMTOI");

	// Get the application path,
	string app_path = QCoreApplication::applicationDirPath().toStdString();
	mShaderSourceDir = app_path + "/shaders/";
	mKernelSourceDir = app_path + "/kernels/";

	mOpenModelDir = "./";
}

/// Opens one saved model file.
void guiMain::Open(QString & filename)
{
	// Attempt to open the file.
	try
	{
		wGLWidget->resetWidget();
		wGLWidget->Open(filename.toStdString());
		wGLWidget->startRendering();
		wGLWidget->Render();
	}
	catch(runtime_error e)
	{
		// An error was thrown. Display a message to the user
		QMessageBox msgBox;
		msgBox.setWindowTitle("SIMTOI Error");
		msgBox.setText(e.what());
		msgBox.exec();

		// Delete the widget and continue to the next file.
		return;
	}

	toggleWidgets();
}

/// Create a new SIMTOI model area and runs the specified minimization engine on the data.  If close_simtoi is true
/// SIMTOI will automatically exit when all minimization engines have completed execution.
void guiMain::run_command_line(QStringList & data_files, QString & model_file,
		string minimizer_id, string save_directory, bool close_simtoi)
{
	Open(model_file);
	wModelParameterEditor->updateModels();
	wOpenDataEditor->openData(data_files);
	wMinimizerWidget->startMinimizer(minimizer_id, save_directory);

	if(close_simtoi)
		connect(wMinimizerWidget, SIGNAL(finished()), this, SLOT(close()));
}

void guiMain::toggleWidgets()
{
    wModelParameterEditor->toggleButtons();
    wOpenDataEditor->toggleButtons();
    wAnimationWidget->toggleButtons();
    wMinimizerWidget->toggleButtons();

	// enable all of the tabs in the bottom
	for(unsigned int i = 0; i < tabBottom->count(); i++)
		tabBottom->setTabEnabled(i, true);
}

void guiMain::on_actionExport_triggered()
{
    // Open a dialog, get a list of file that the user selected:
 	QFileDialog dialog(this);
 	dialog.setFileMode(QFileDialog::Directory);
 	dialog.setOption(QFileDialog::ShowDirsOnly);

    QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		// Make the directory if it does not exist
		if(!QDir(filenames[0]).exists())
			QDir().mkdir(filenames[0]);

		wGLWidget->Export(filenames[0]);
	}
}

void guiMain::on_actionOpen_triggered()
{
   // Open a dialog, get a list of file that the user selected:
	QFileDialog dialog(this);
	dialog.setDirectory(QString::fromStdString(mOpenModelDir));
	dialog.setNameFilter(tr("SIMTOI Model Files (*.json)"));
	dialog.setFileMode(QFileDialog::ExistingFile);

    QString dir = "";
	if (dialog.exec())
	{
	    QStringList filenames = dialog.selectedFiles();
		Open(filenames[0]);
	}
}

/// Opens up a dialog for creating a new model region
void guiMain::on_actionNew_triggered(void)
{
	guiRegion dialog;
	if(dialog.exec())
	{
		unsigned int width = dialog.spinWidth->value();
		unsigned int height = dialog.spinHeight->value();
		double scale = dialog.spinScale->value();

		// reset the GL widget
		wGLWidget->resetWidget();
		wGLWidget->SetSize(width, height);
		wGLWidget->SetScale(scale);
		wGLWidget->startRendering();

		toggleWidgets();
	}
}

void guiMain::on_actionSave_triggered()
{
    string filename;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("SIMTOI Save Files (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList fileNames;
	if (dialog.exec())
	{
		fileNames = dialog.selectedFiles();
		filename = fileNames.first().toStdString();
		string tmp =filename.substr(filename.size() - 5, 5);

		if(filename.substr(filename.size() - 5, 5) != ".json")
			filename += ".json";

		wGLWidget->Save(filename);
	}

}
