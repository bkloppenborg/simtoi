/*
 * wMinimizer.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wMinimizer.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

#include "guiCommon.h"
#include "CMinimizerFactory.h"
#include "CMinimizerThread.h"
#include "CGLWidget.h"
#include "CModelList.h"

wMinimizer::wMinimizer(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	mGLWidget = NULL;

	// Setup the combo boxes.
	guiCommon::setOptions(this->cboMinimizers, CMinimizerFactory::getInstance().getNames());

	// Setup the text boxes
	setSaveDirectory("/tmp/model");

	setEnabled(false);
}

wMinimizer::~wMinimizer()
{
	if(mMinimizer)
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}
}

/// \brief Run specific operations when a minimizer is started.
///
/// Toggles the start/stop button and emits the `started` signal.
void wMinimizer::minimizerStarted()
{
	btnStartStop->setText(QString("Stop"));
	emit started();
}

/// \brief Run specific operations when a minimizer has finished.
///
/// Toggles the start/stop button and emits the `finished` signal.
void wMinimizer::minimizerFinished()
{
	btnStartStop->setText(QString("Start"));
	emit finished();
}

///	\brief Start or stop the minimization engine when btnStartStop is clicked
void wMinimizer::on_btnStartStop_clicked()
{
	/// TODO: Handle thread-execution exceptions that might be thrown.
	if(!mGLWidget)
		return;

	// Determine the save-file directory
	QString save_dir = textSaveFolder->text();
	// make the directory if it doesn't exist
	if(!QDir(save_dir).exists())
		QDir().mkdir(save_dir);

	string save_directory = save_dir.toStdString();

	// If the minimizer is running, stop it.
	if(mMinimizer && mMinimizer->isRunning())
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}
	else // start a new minimizer.
	{
		// find the ID for the minimizer
		string name = this->cboMinimizers->currentText().toStdString();
		string id = CMinimizerFactory::getInstance().idFromName(name);

		// start the minimizer
		startMinimizer(id, save_directory);
	}
}

/// Sets the current widget. Connects necessary signals and slots.
void wMinimizer::setGLWidget(CGLWidget * gl_widget)
{
	mGLWidget = gl_widget;

//	toggleButtons();
}


/// Sets the desired save directory. Creates the directory if it does not exist.
void wMinimizer::setSaveDirectory(const string & save_directory)
{
	QString q_save_directory = QString::fromStdString(save_directory);
	// Replace tilde with the user's home directory
	if (q_save_directory.startsWith ("~/"))
		q_save_directory.replace(0, 1, QDir::homePath());

	qDebug() << q_save_directory;
	// verify the save directory exists, if not create it
	if(!QDir(q_save_directory).exists())
	{
		QDir().mkpath(q_save_directory);
	}

	// verify that we can write to the directory
	QFileInfo fi(q_save_directory);
	if(!fi.isWritable())
		throw runtime_error("The directory '" + save_directory + "' is not writable. Please check permissions.");

	textSaveFolder->setText(q_save_directory);
	mSaveDirectory = q_save_directory.toStdString();
}

/// Starts a minimization
///
/// In order to start the minimization engine, the following must hold
///  * At least one data file is loaded
///	 * At least one model exists
/// If one of these is not true, a dialog will be displayed to the user and
/// no minimization engine will be started.
///
/// \param minimizer_id The desired minimizer. See CMinimizerFactory for string names
/// \param save_directory The directory in which temporary output data should be saved.
void wMinimizer::startMinimizer(const string & minimizer_id, const string & save_directory)
{
	// stop any presently running minimization engine.
	if(mMinimizer && mMinimizer->isRunning())
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}

	// verify data is loaded
	if(mGLWidget->getNData() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("You must load data prior to starting a minimizer"));
		msgBox.exec();
		return;
	}

	// verify that there are models
	if(mGLWidget->getNModels() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("You must define a model prior to starting a minimizer"));
		msgBox.exec();
		return;
	}

	// check that there is at least one free parameter
	if(mGLWidget->getModels()->GetNFreeParameters() < 1)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Your models must have at least 1 free parameter to start a minimizer"));
		msgBox.exec();
		return;
	}

	// try to start the minimizer
	try
	{
		setSaveDirectory(save_directory);

		auto worker = mGLWidget->getWorker();

		mMinimizer = CMinimizerFactory::getInstance().create(minimizer_id);
		mMinimizer->setSaveDirectory(mSaveDirectory);
		mMinimizer->Init(worker);
		mMinimizer->start();

		// the minimizer started, update the minimizer ID combo box
		QString q_minimizer_id = QString::fromStdString(minimizer_id);
		int id = this->cboMinimizers->findText(q_minimizer_id);
		if(id > -1)
			this->cboMinimizers->setCurrentIndex(id);
		// signal that the minimization started.
		minimizerStarted();

		connect(mMinimizer.get(), SIGNAL(finished()), this, SLOT(minimizerFinished()));
	}
	catch(runtime_error & e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Could not start minimizer.\n") + QString(e.what()));
		msgBox.exec();
	}
}

/// Automatically (de)acitvate buttons depending on the status of various properties
/// of this widget.
void wMinimizer::toggleButtons()
{
	this->setEnabled(false);

	if(mGLWidget)
		this->setEnabled(true);
}
