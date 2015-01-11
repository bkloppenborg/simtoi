/*
 * wMinimizer.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wMinimizer.h"
#include "guiCommon.h"
#include "CMinimizerFactory.h"

#include "CGLWidget.h"
#include "CMinimizerThread.h"

wMinimizer::wMinimizer(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	// Setup the combo boxes.
	auto minimizers = CMinimizerFactory::Instance();
	guiCommon::setOptions(this->cboMinimizers, minimizers.GetMinimizerList());

	// Setup the text boxes
	mDefaultSaveDir = "/tmp/model";
	this->textSaveFolder->setText(mDefaultSaveDir.c_str());


//	// connect any non-automatic signal/slots
//	connect(&mAnimator, SIGNAL(update_time(double)), this, SLOT(update_time(double)));
//	connect(this, SIGNAL(timestep_updated(double)), &mAnimator, SLOT(setStep(double)));

	setEnabled(false);
}

wMinimizer::~wMinimizer()
{
//	mAnimator.stop();
//	mAnimator.wait();
}



/// Sets the current widget. Connects necessary signals and slots.
void wMinimizer::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

//	mAnimator.stop();
//	mAnimator.setGLWidget(gl_widget);

	toggleButtons();
}

/// Automatically (de)acitvate buttons depending on the status of various properties
/// of this widget.
void wMinimizer::toggleButtons()
{
	this->setEnabled(false);

	if(mGLWidget)
		this->setEnabled(true);
}
//
///// Checks to see which buttons can be enabled/disabled.
//void gui_main::ButtonCheck()
//{
//	if(!mGLWidget)
//		return;
//
//	// Buttons for minimizer area
//	// Look up the minimizer's ID in the combo box, select it.
//	int id = this->cboMinimizers->findText(QString::fromStdString(mGLWidget->GetMinimizerID()));
//	if(id > -1)
//		this->cboMinimizers->setCurrentIndex(id);
//
//	// Toggle minimizer start/stop button
//	if(mGLWidget->GetMinimizerRunning())
//		this->btnMinimizerStartStop->setText("Stop");
//	else
//		this->btnMinimizerStartStop->setText("Start");
//
//}


//void gui_main::MinimizerRun(string MinimizerID)
//{
//	// Look up the minimizer
//	auto minimizers = CMinimizerFactory::Instance();
//
//	// Determine the save-file directory
//	QString save_dir = textSaveFolder->text();
//	// make the directory if it doesn't exist
//	if(!QDir(save_dir).exists())
//		QDir().mkdir(save_dir);
//	// set the path in the widget.
//	mGLWidget->SetSaveDirectory(save_dir.toStdString());
//
//	try
//	{
//		CMinimizerPtr minimizer = minimizers.CreateMinimizer(MinimizerID);
//		mGLWidget->SetMinimizer(minimizer);
//		mGLWidget->startMinimizer();
//	}
//	catch(runtime_error & e)
//	{
//		QMessageBox msgBox;
//		msgBox.setText(QString("Could not start minimizer.\n") + QString(e.what()));
//		msgBox.exec();
//	}
//
//	ButtonCheck();
//}

//void gui_main::SetOutputDir(string folder_name)
//{
//	mDefaultSaveDir = folder_name;
//
//	this->textSaveFolder->setText(mDefaultSaveDir.c_str());
//}
//
//void gui_main::minimizerFinished()
//{
//	// Update the buttons
//	ButtonCheck();
//	TreeCheck();
//
//	// Close the subwindow if automatic closing is enabled.
//	if(mAutoClose)
//		close();
//}

///// Starts the minimizer
//void gui_main::on_btnMinimizerStartStop_clicked()
//{
//	/// TODO: Handle thread-execution exceptions that might be thrown.
//	if(!mGLWidget)
//		return;
//
//	// If the minimizer is running, stop it.
//	if(mGLWidget->GetMinimizerRunning())
//		mGLWidget->stopMinimizer();
//	else // start a new minimizer.
//	{
//		string id = this->cboMinimizers->currentText().toStdString();
//
//		QAbstractItemModel * model;
//
////		// Ensure that a model exists before starting the minimizer
////		CModelPtr model_ptr = mGLWidget->getModel(0);
////		if(!model_ptr)
////		{
////			QMessageBox msgBox;
////			msgBox.setWindowTitle("Error");
////			msgBox.setText("Please add a model before starting the minimization engine");
////			msgBox.exec();
////
////			return;
////		}
//
////		// Ensure that some data is loaded before starting the minimizer.
////		if(mGLWidget->GetOpenFileModel()->rowCount() > 0)
////		{
////			QMessageBox msgBox;
////			msgBox.setWindowTitle("Error");
////			msgBox.setText("Please load data before starting the minimization engine");
////			msgBox.exec();
////
////			return;
////		}
//
//		MinimizerRun(id);
//	}
//
//	ButtonCheck();
//}
