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

#include "CGLWidget.h"
#include "CPosition.h"
#include "CMinimizerThread.h"
#include "CTreeModel.h"
#include "gui_common.h"
#include "gui_model.h"
#include "CMinimizerFactory.h"

gui_main::gui_main(QWidget *parent_widget)
    : QMainWindow(parent_widget)
{
	Init();
}

gui_main::~gui_main()
{
	close();
}

void gui_main::AddData(QStringList & filenames, QMdiSubWindow * sw)
{
	string tmp;
	int dir_size = 0;
	stringstream time_str;
	time_str.precision(4);
	time_str.setf(ios::fixed,ios::floatfield);

    // Get access to the current widget and QStandardItemModel:
    CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
    QStandardItemModel * model = widget->GetOpenFileModel();
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
		widget->OpenData(tmp);

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

QMdiSubWindow * gui_main::AddGLArea(CGLWidget * gl_widget)
{
	// Set the widget's parent to be the MDI area:
	gl_widget->setParent(this->mdiArea);

	// Create a subwindow in which the widget can live:
    QMdiSubWindow * sw = this->mdiArea->addSubWindow(gl_widget, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    sw->setWindowTitle("Model Area");

    // Set the subwindow's size
    // TODO: This is approximately right for my machine, probably not ok on other OSes.
    int frame_width = 8;
    int frame_height = 28;

    sw->setFixedSize(gl_widget->GetImageWidth() + frame_width, gl_widget->GetImageHeight() + frame_height);
    sw->show();

    // Start the widget rendering:
    gl_widget->startRendering();

    // Connect signals/slots
	// Now connect signals and slots
	connect(gl_widget->GetTreeModel(), SIGNAL(parameterUpdated(void)), this, SLOT(render(void)));
	connect(gl_widget, SIGNAL(minimizerFinished(void)), this, SLOT(AutoClose()));

	// If the load data button isn't enabled, turn it on
	ButtonCheck();

	return sw;
}

void gui_main::Animation_StartStop()
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());

//	if(mAnimating)
//	{
//		widget->EnqueueOperation(GLT_AnimateStop);
//		mAnimating = false;
//		this->btnStartStop->setText("Start");
//	}
//	else
//	{
//		widget->SetTimestep(this->spinTimeStep->value());
//		widget->EnqueueOperation(GLT_Animate);
//		mAnimating = true;
//		this->btnStartStop->setText("Stop");
//	}
}

void gui_main::Animation_Reset()
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());

//	widget->SetTime(this->spinTimeStart->value());
//	widget->EnqueueOperation(GLT_RenderModels);

}

void gui_main::AutoClose()
{
	QMdiSubWindow * sw = dynamic_cast<QMdiSubWindow*>(sender());

	if(mAutoClose)
	{
		// Close the calling subwindow
		sw->close();

		// If there are more open subwindows and auto-close is set, close SIMTOI
		if(this->mdiArea->subWindowList().size() == 0)
			close();
	}
}

/// Checks to see which buttons can be enabled/disabled.
void gui_main::ButtonCheck()
{

	this->btnAddData->setEnabled(false);
	this->btnAddModel->setEnabled(false);
	this->btnEditModel->setEnabled(false);
	this->btnDeleteModel->setEnabled(false);
	this->btnRemoveData->setEnabled(false);
	this->btnStartStop->setEnabled(false);
	this->btnReset->setEnabled(false);
	this->btnMinimizerStart->setEnabled(false);
	this->btnMinimizerStop->setEnabled(false);
	this->btnSavePhotometry->setEnabled(false);
	this->btnSaveFITS->setEnabled(false);
	this->btnSetTime->setEnabled(false);

    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

    // Animation / data export area
	this->btnStartStop->setEnabled(true);
	this->btnReset->setEnabled(true);
	this->btnSavePhotometry->setEnabled(true);
	this->btnSaveFITS->setEnabled(true);
	this->btnSetTime->setEnabled(true);

	// Buttons for add/delete data
	this->btnAddData->setEnabled(true);
	CGLWidget * widget = dynamic_cast<CGLWidget *>(sw->widget());
	if(widget->GetOpenFileModel()->rowCount() > 0)
		this->btnRemoveData->setEnabled(true);

	// Buttons for add/edit/delete model
	this->btnAddModel->setEnabled(true);
	if(widget->GetTreeModel()->rowCount() > 0)
	{
		this->btnEditModel->setEnabled(true);
		this->btnDeleteModel->setEnabled(true);
	}

	// Buttons for minimizer area
	// Look up the minimizer's ID in the combo box, select it.
	int id = this->cboMinimizers->findText(QString::fromStdString(widget->GetMinimizerID()));
	if(id > -1)
		this->cboMinimizers->setCurrentIndex(id);
	// Toggle minimizer start/stop buttons
	if(widget->GetMinimizerRunning())
	{
		this->btnMinimizerStop->setEnabled(true);
		this->btnMinimizerStart->setEnabled(false);
	}
	else
	{
		this->btnMinimizerStop->setEnabled(false);
		this->btnMinimizerStart->setEnabled(true);
	}

}

void gui_main::close()
{
	CGLWidget * widget = NULL;
	QMdiSubWindow * sw = NULL;

	QList<QMdiSubWindow *> windows = this->mdiArea->subWindowList();
    for (int i = int(windows.count()) - 1; i > 0; i--)
    {
    	QMdiSubWindow * sw = windows.at(i);
    	CGLWidget * widget = dynamic_cast<CGLWidget *>(sw->widget());
    	widget->stopRendering();
    	sw->close();
    }

    // Now call the base-class close method.
    QMainWindow::close();
}

void gui_main::closeEvent(QCloseEvent *evt)
{
	close();
    QMainWindow::closeEvent(evt);
}

/// Create a new SIMTOI model area and runs the specified minimization engine on the data.  If close_simtoi is true
/// SIMTOI will automatically exit when all minimization engines have completed execution.
void gui_main::CommandLine(QStringList & data_files, QStringList & model_files, string minimizer, bool close_simtoi)
{
	// First open the model area and configure the UI.
	Open(model_files);

	// Get a pointer to the active model area.
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

    // Now open data, run the minimizer and close (if needed)
	AddData(data_files, sw);
	MinimizerRun(minimizer, sw);
	mAutoClose = close_simtoi;
}

/// Exports the current rendered model to a floating point FITS file
void gui_main::ExportFITS()
{
//    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
//    if(!sw)
//    	return;
//
//	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
////	widget->EnqueueOperation(CLT_Init);
//
//    string filename;
//    QStringList fileNames;
//    QFileDialog dialog(this);
//    dialog.setFileMode(QFileDialog::AnyFile);
//    dialog.setNameFilter(tr("FITS Files (*.fits)"));
//    dialog.setViewMode(QFileDialog::Detail);
//    dialog.setAcceptMode(QFileDialog::AcceptSave);
//
//	if (dialog.exec())
//	{
//
//		fileNames = dialog.selectedFiles();
//		filename = fileNames.first().toStdString();
//
//		// Add an extension if it doesn't already exist
//		if(filename.substr(filename.size() - 5, 5) != ".fits")
//			filename += ".fits";
//
//		// Automatically overwrite files if they already exist.  The dialog should prompt for us.
//		filename = "!" + filename;
//
//		widget->SaveImage(filename);
//	}
}

/// Animates the display, exporting the photometry at specified intervals
void gui_main::ExportPhotometry()
{
//    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
//    if(!sw)
//    	return;
//
//	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
////	widget->EnqueueOperation(CLT_Init);
//
//    double t = this->spinTimeStart->value();
//    double step = this->spinTimeStep->value();
//    double duration = this->spinTimeDuration->value();
//    double stop = t + duration;
//    vector< pair<float, float> > output;
//    float flux;
//
//    string filename;
//    QStringList fileNames;
//    QFileDialog dialog(this);
//    dialog.setFileMode(QFileDialog::AnyFile);
//    dialog.setNameFilter(tr("Text Files (*.txt)"));
//    dialog.setViewMode(QFileDialog::Detail);
//    dialog.setAcceptMode(QFileDialog::AcceptSave);
//
//	if (dialog.exec())
//	{
//		// Compute the flux
//		while(t < stop)
//		{
////			widget->SetTime(t);
////			widget->EnqueueOperation(GLT_RenderModels);
////			flux = widget->GetFlux();
//
//			output.push_back( pair<float, float>(t, flux));
//			t += step;
//		}
//
//		fileNames = dialog.selectedFiles();
//		filename = fileNames.first().toStdString();
//
//		if(filename.substr(filename.size() - 4, 4) != ".txt")
//			filename += ".txt";
//	}
//
//	ofstream outfile;
//	outfile.open(filename.c_str());
//	outfile.precision(4);
//	outfile.setf(ios::fixed,ios::floatfield);
//	outfile << "# Time Flux" << endl;
//	//outfile << "# Created using the following parameters: " << endl;
//
//	for(int i = 0; i < output.size(); i++)
//	{
//		outfile << output[i].first << " " << -2.5*log10(output[i].second) << endl;
//	}
//
//	outfile.close();

}

/// Runs initialization routines for the main this->
void gui_main::Init(void)
{
	// Init the UI
	this->setupUi(this);

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
	gui_common::SetupComboOptions(this->cboMinimizers, minimizers.GetMinimizerList());

	// Setup the text boxes
	this->textSavePath->setText(mDefaultSaveDir.c_str());
}

void gui_main::MinimizerRun(string MinimizerID, QMdiSubWindow * sw)
{
	// We have a valid subwindow, so cast it into a CGLWidget
	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());

	// Look up the minimizer
	auto minimizers = CMinimizerFactory::Instance();

	try
	{
		CMinimizerPtr minimizer = minimizers.CreateMinimizer(MinimizerID);
		widget->SetMinimizer(minimizer);
		widget->startMinimizer();
	}
	catch(runtime_error & e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Could not start minimizer.\n") + QString(e.what()));
		msgBox.exec();
	}

	ButtonCheck();
}

void gui_main::Open(QStringList & filenames)
{
	for(auto filename : filenames)
	{
		CGLWidget * widget = new CGLWidget(NULL, mShaderSourceDir, mKernelSourceDir);
		widget->Open(filename.toStdString());
		AddGLArea(widget);
	}
}



void gui_main::on_actionOpen_triggered()
{
   // Open a dialog, get a list of file that the user selected:
	QFileDialog dialog(this);
	dialog.setDirectory(QString::fromStdString(mOpenDataDir));
	dialog.setNameFilter(tr("SIMTOI Model Files (*.json)"));
	dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList filenames;
    QString dir = "";
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		Open(filenames);
	}
}

void gui_main::on_actionSave_triggered()
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
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

		CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
		widget->Save(filename);
	}

}

void gui_main::on_btnAddData_clicked(void)
{
	// Ensure there is a selected widget, if not immediately return.
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
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
    CGLWidget * widget = dynamic_cast<CGLWidget*>(sw->widget());
    QStringList filters = widget->GetFileFilters();
    dialog.setNameFilters(filters);

    QStringList filenames;
    QString dir = "";
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		AddData(filenames, sw);
	}
}

void gui_main::on_btnAddModel_clicked(void)
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
    int id = 0;
    int n_features;

    gui_model tmp;
    tmp.show();

    if(tmp.exec())
    {
    	widget->AddModel(tmp.GetModel());
    }

    // Now render the models and refresh the tree
//    widget->EnqueueOperation(GLT_RenderModels);

}

/// Deletes the selected model from the model list
void gui_main::on_btnDeleteModel_clicked()
{
	// TODO: write this function
}

/// Opens up an editing dialog for the currently selected model.
void gui_main::on_btnEditModel_clicked()
{
	// TODO: write this function
}

/// Starts the minimizer
void gui_main::on_btnMinimizerStart_clicked()
{
	/// TODO: Handle thread-execution exceptions that might be thrown.

    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	// We have a valid subwindow, so cast it into a CGLWidget
	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());

	// Look up the minimizer
	string id = this->cboMinimizers->currentText().toStdString();
	MinimizerRun(id, sw);
}

/// Stops the minimizer running on the currently selected subwindow
void gui_main::on_btnMinimizerStop_clicked()
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
	widget->stopMinimizer();
	ButtonCheck();
}

void gui_main::on_mdiArea_subWindowActivated()
{
	ButtonCheck();
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());

	// Configure the open file widget:
	this->treeOpenFiles->setHeaderHidden(false);
    this->treeOpenFiles->setModel(widget->GetOpenFileModel());
	this->treeOpenFiles->header()->setResizeMode(QHeaderView::ResizeToContents);

	// Configure the model tree
    CTreeModel * model = widget->GetTreeModel();
	this->treeModels->setHeaderHidden(false);
	this->treeModels->setModel(widget->GetTreeModel());
	this->treeModels->header()->setResizeMode(QHeaderView::ResizeToContents);
}

void gui_main::on_btnNewModelArea_clicked()
{
	int width = this->spinModelSize->value();
    int height = this->spinModelSize->value();
    double scale = this->spinModelScale->value();

	CGLWidget * widget = new CGLWidget(NULL, mShaderSourceDir, mKernelSourceDir);
	widget->SetSize(width, height);
	widget->SetScale(scale);
	AddGLArea(widget);
}

/// Removes the current selected data set.
void gui_main::on_btnRemoveData_clicked()
{
	// Ensure there is a selected widget, if not immediately return.
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

    // Get access to the current widget, and QStandardItemModel list
    CGLWidget * widget = dynamic_cast<CGLWidget *>(sw->widget());

    // Get the selected indicies:
    QModelIndexList list = this->treeOpenFiles->selectionModel()->selectedIndexes();
    QStandardItemModel * model = widget->GetOpenFileModel();

    QList<QModelIndex>::iterator it;
    int id = 0;
    for(it = --list.end(); it > list.begin(); it--)
    {
    	id = (*it).row();
//    	widget->RemoveData(id);
    	model->removeRow(id, QModelIndex());
    }
}

void gui_main::render()
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

	CGLWidget *widget = dynamic_cast<CGLWidget*>(sw->widget());
    if(widget)
    {
//    	widget->EnqueueOperation(GLT_RenderModels);
    }
}

void gui_main::SetSavePath(void)
{

}

/// Sets the time from the current selected datafile.
void gui_main::SetTime(void)
{
    QMdiSubWindow * sw = this->mdiArea->activeSubWindow();
    if(!sw)
    	return;

    // Get access to the current widget, and QStandardItemModel list
    CGLWidget * widget = dynamic_cast<CGLWidget *>(sw->widget());

    // Get the selected indicies:
    QModelIndexList list = this->treeOpenFiles->selectionModel()->selectedIndexes();
    QStandardItemModel * model = widget->GetOpenFileModel();

    // Ensure there is data present before continuing.
    if(list.size() == 0)
    	return;

    QList<QModelIndex>::iterator it = list.begin();
    int id = (*it).row();
//    double t = widget->GetDataAveJD(id);
//    widget->SetTime(t);
//    widget->EnqueueOperation(GLT_RenderModels);
}

