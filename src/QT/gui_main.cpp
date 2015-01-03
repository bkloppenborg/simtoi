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
 
#include "gui_main.h"

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
#include "CGLWidget.h"
#include "guiCommon.h"
#include "guiRegion.h"
#include "CTreeModel.h"
#include "CMinimizerThread.h"

#include "CPosition.h"

#include "CMinimizerFactory.h"
#include "wParameterEditor.h"

gui_main::gui_main(QWidget *parent_widget)
    : QMainWindow(parent_widget)
{
	Init();
}

gui_main::~gui_main()
{
	close();
}

void gui_main::AddData(QStringList & filenames)
{
	string tmp;
	int dir_size = 0;
	stringstream time_str;
	time_str.precision(4);
	time_str.setf(ios::fixed,ios::floatfield);

    // Get access to the current widget and QStandardItemModel:
    QStandardItemModel * model = mGLWidget->GetOpenFileModel();
	QList<QStandardItem *> items;

	// Now pull out the data directory (to make file display cleaner)
	if(filenames.size() > 0)
		mOpenDataDir = QFileInfo(filenames[0]).absolutePath().toStdString();

	for(int i = 0; i < filenames.size(); i++)
	{
		items.clear();
		// Tell the widget to load the data file and append a row to its file list:
		tmp = filenames[i].toStdString();
		dir_size = tmp.size() - mOpenDataDir.size();
		mGLWidget->OpenData(tmp);

		// Filename
		items.append( new QStandardItem(QString::fromStdString( tmp.substr(mOpenDataDir.size() + 1, dir_size) )));
		// Mean JD
		time_str.str("");
		// TODO: Re-enable this (or something like it).
//		time_str << widget->GetDataAveJD(widget->GetNDataSets() - 1);
		items.append( new QStandardItem(QString::fromStdString( time_str.str() )));

		model->appendRow(items);
	}

	ButtonCheck();
}

void gui_main::AddGLArea(CGLWidgetPtr gl_widget)
{
	// TODO: Rework once GLWidgets are persistent.
	if(mGLWidget != NULL)
	{
		this->topRightLayout->removeWidget(mGLWidget.get());
		// TODO: Delete animation widget
//		this->tabBottom->removeWidget(mAnimationWidget);
	}

	// Create a new GLWidget and start it rendering
	mGLWidget = gl_widget;
	this->topRightLayout->addWidget(mGLWidget.get());
	mGLWidget->setFixedSize(mGLWidget->GetImageWidth(), gl_widget->GetImageHeight());
	mGLWidget->show();
    mGLWidget->startRendering();

    // TODO: Rework once GLWidgets are persistent.
	// populate the left layout
    this->wModelParameterEditor->setGLWidget(mGLWidget);

    // Create a new animation widget, init the tab region
    wAnimationWidget = new wAnimation(mGLWidget);
	this->tabBottom->addTab(wAnimationWidget, QString("Animation"));

    // Connect signals/slots
	// Now connect signals and slots
	connect(mGLWidget.get(), SIGNAL(minimizerFinished(void)), this, SLOT(minimizerFinished(void)));

	// If the load data button isn't enabled, turn it on
	ButtonCheck();

	// enable all of the tabs in the bottom
	for(unsigned int i = 0; i < tabBottom->count(); i++)
		tabBottom->setTabEnabled(i, true);

}

/// Checks to see which buttons can be enabled/disabled.
void gui_main::ButtonCheck()
{
	this->btnAddData->setEnabled(false);
	this->btnRemoveData->setEnabled(false);
	this->btnMinimizerStartStop->setEnabled(true);

	if(!mGLWidget)
		return;

	// Buttons for add/delete data
	this->btnAddData->setEnabled(true);
	if(mGLWidget->GetOpenFileModel()->rowCount() > 0)
		this->btnRemoveData->setEnabled(true);

	// Buttons for minimizer area
	// Look up the minimizer's ID in the combo box, select it.
	int id = this->cboMinimizers->findText(QString::fromStdString(mGLWidget->GetMinimizerID()));
	if(id > -1)
		this->cboMinimizers->setCurrentIndex(id);

	// Toggle minimizer start/stop button
	if(mGLWidget->GetMinimizerRunning())
		this->btnMinimizerStartStop->setText("Stop");
	else
		this->btnMinimizerStartStop->setText("Start");

}

void gui_main::close()
{
    mGLWidget->stopRendering();
    QMainWindow::close();
}

void gui_main::closeEvent(QCloseEvent *evt)
{
	close();
    QMainWindow::closeEvent(evt);
}

/// Create a new SIMTOI model area and runs the specified minimization engine on the data.  If close_simtoi is true
/// SIMTOI will automatically exit when all minimization engines have completed execution.
void gui_main::CommandLine(QStringList & data_files, QString & model_file, string minimizer, bool close_simtoi)
{
	// First open the model area and configure the UI.
	Open(model_file);

    // Now open data, run the minimizer and close (if needed)
	AddData(data_files);
	MinimizerRun(minimizer);
	mAutoClose = close_simtoi;
}

/// Runs initialization routines for the main this->
void gui_main::Init(void)
{
	// Init the UI
	this->setupUi(this);
	this->setWindowTitle("SIMTOI");

	// Now init variables:

	mAnimating = false;
	mAutoClose = false;

	// Get the application path,
	string app_path = QCoreApplication::applicationDirPath().toStdString();
	mShaderSourceDir = app_path + "/shaders/";
	mKernelSourceDir = app_path + "/kernels/";

	mOpenDataDir = "./";
	mOpenModelDir = "./";

	mDefaultSaveDir = "/tmp/model";
	mNumMinimizations = 0;

	// Setup the combo boxes.
	auto minimizers = CMinimizerFactory::Instance();
	guiCommon::setOptions(this->cboMinimizers, minimizers.GetMinimizerList());

	// Setup the text boxes
	this->textSaveFolder->setText(mDefaultSaveDir.c_str());
}

void gui_main::MinimizerRun(string MinimizerID)
{
	// Look up the minimizer
	auto minimizers = CMinimizerFactory::Instance();

	// Determine the save-file directory
	QString save_dir = textSaveFolder->text();
	// make the directory if it doesn't exist
	if(!QDir(save_dir).exists())
		QDir().mkdir(save_dir);
	// set the path in the widget.
	mGLWidget->SetSaveDirectory(save_dir.toStdString());

	try
	{
		CMinimizerPtr minimizer = minimizers.CreateMinimizer(MinimizerID);
		mGLWidget->SetMinimizer(minimizer);
		mGLWidget->startMinimizer();
	}
	catch(runtime_error & e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Could not start minimizer.\n") + QString(e.what()));
		msgBox.exec();
	}

	ButtonCheck();
}

/// Opens one saved model file.
void gui_main::Open(QString & filename)
{
	CGLWidgetPtr widget = CGLWidgetPtr(new CGLWidget(NULL, mShaderSourceDir, mKernelSourceDir));

	// Attempt to open the file.
	try
	{
		widget->Open(filename.toStdString());
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

	// If we opened the file successfully,
	AddGLArea(widget);
}

void gui_main::SetOutputDir(string folder_name)
{
	mDefaultSaveDir = folder_name;

	this->textSaveFolder->setText(mDefaultSaveDir.c_str());
}

void gui_main::minimizerFinished()
{
	// Update the buttons
	ButtonCheck();
	TreeCheck();

	// Close the subwindow if automatic closing is enabled.
	if(mAutoClose)
		close();
}

void gui_main::on_actionExport_triggered()
{
    if(!mGLWidget)
    	return;

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

		mGLWidget->Export(filenames[0]);
	}
}

void gui_main::on_actionOpen_triggered()
{
   // Open a dialog, get a list of file that the user selected:
	QFileDialog dialog(this);
	dialog.setDirectory(QString::fromStdString(mOpenDataDir));
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
void gui_main::on_actionNew_triggered(void)
{
	guiRegion dialog;
	if(dialog.exec())
	{
		unsigned int width = dialog.spinWidth->value();
		unsigned int height = dialog.spinHeight->value();
		double scale = dialog.spinScale->value();

		CGLWidgetPtr widget = CGLWidgetPtr(new CGLWidget(NULL, mShaderSourceDir, mKernelSourceDir));
		widget->SetSize(width, height);
		widget->SetScale(scale);
		AddGLArea(widget);
	}
}

void gui_main::on_actionSave_triggered()
{
    if(!mGLWidget)
    	return;

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

		mGLWidget->Save(filename);
	}

}

void gui_main::on_btnAddData_clicked(void)
{
	// Ensure there is a selected widget, if not immediately return.
    if(!mGLWidget)
    {
		QMessageBox msgBox;
		msgBox.setText("You must have an active model region before you can load data.");
		msgBox.exec();
    	return;
    }

    // Open a dialog, get a list of file that the user selected:
    QFileDialog dialog(this);
    dialog.setDirectory(QString::fromStdString(mOpenDataDir));
    dialog.setFileMode(QFileDialog::ExistingFiles);

    // Now add in valid file types:
    QStringList filters = mGLWidget->GetFileFilters();
    dialog.setNameFilters(filters);

    QStringList filenames;
    QString dir = "";
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		AddData(filenames);
	}
}

/// Starts the minimizer
void gui_main::on_btnMinimizerStartStop_clicked()
{
	/// TODO: Handle thread-execution exceptions that might be thrown.
	if(!mGLWidget)
		return;

	// If the minimizer is running, stop it.
	if(mGLWidget->GetMinimizerRunning())
		mGLWidget->stopMinimizer();
	else // start a new minimizer.
	{
		string id = this->cboMinimizers->currentText().toStdString();

		QAbstractItemModel * model;

//		// Ensure that a model exists before starting the minimizer
//		CModelPtr model_ptr = mGLWidget->getModel(0);
//		if(!model_ptr)
//		{
//			QMessageBox msgBox;
//			msgBox.setWindowTitle("Error");
//			msgBox.setText("Please add a model before starting the minimization engine");
//			msgBox.exec();
//
//			return;
//		}

//		// Ensure that some data is loaded before starting the minimizer.
//		if(mGLWidget->GetOpenFileModel()->rowCount() > 0)
//		{
//			QMessageBox msgBox;
//			msgBox.setWindowTitle("Error");
//			msgBox.setText("Please load data before starting the minimization engine");
//			msgBox.exec();
//
//			return;
//		}

		MinimizerRun(id);
	}

	ButtonCheck();
}

/// Removes the current selected data set.
void gui_main::on_btnRemoveData_clicked()
{
	if(!mGLWidget)
		return;

    // Get the selected indicies:
    QModelIndexList list = this->treeOpenFiles->selectionModel()->selectedIndexes();
    QStandardItemModel * model = mGLWidget->GetOpenFileModel();

    QList<QModelIndex>::iterator it;
    int id = 0;
    for(it = --list.end(); it > list.begin(); it--)
    {
    	id = (*it).row();
//    	mGLWidget->RemoveData(id);
    	model->removeRow(id, QModelIndex());
    }
}

void gui_main::TreeCheck()
{
	// Configure the open file widget:
	this->treeOpenFiles->setHeaderHidden(false);
	this->treeOpenFiles->setModel(mGLWidget->GetOpenFileModel());
	this->treeOpenFiles->header()->setResizeMode(QHeaderView::ResizeToContents);
}
