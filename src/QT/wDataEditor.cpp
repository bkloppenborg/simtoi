/*
 * wDataEditor.cpp
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
 */

#include "wDataEditor.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>

#include "CGLWidget.h"
#include "CDataInfo.h"

wDataEditor::wDataEditor(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	mGLWidget = NULL;

	QStringList labels = QStringList();
	labels << "Filename" << "Quantity" << "JD Start" << "JD End" << "Mean JD" << QString::fromUtf8("Mean λ (μm)") << "chi2r";
	mOpenFileModel.clear();
	mOpenFileModel.setColumnCount(7);
	mOpenFileModel.setHorizontalHeaderLabels(labels);
}

wDataEditor::~wDataEditor()
{

}

/// Adds information related to the specified data to the text entry box
void wDataEditor::addData(const CDataInfo & data_info)
{
	QList<QStandardItem *> items;
	QString temp;
	double value;

	unsigned int precision = 4;

	// filename
	temp = QString::fromUtf8(data_info.filename().c_str());
	items.append(new QStandardItem(temp));
	// number of data
	temp = QString::fromUtf8(data_info.description().c_str());
	items.append(new QStandardItem(temp));
	// starting julian date
	value = data_info.JDMin();
	if(value > 0)
	{
		temp = QString::number(value, 'f', precision);
		items.append(new QStandardItem(temp));
	}
	else
		items.append(new QStandardItem(QString()));

	// ending julian date
	value = data_info.JDMax();
	if(value > 0)
	{
		temp = QString::number(value, 'f', precision);
		items.append(new QStandardItem(temp));
	}
	else
		items.append(new QStandardItem(QString()));

	// mean julian date
	temp = QString::number(data_info.JDMean(), 'f', precision);
	items.append(new QStandardItem(temp));
	// mean wavelength
	temp = QString::number(data_info.wavelengthMean() * 1E6, 'f', precision); // convert from meters to micrometers
	items.append(new QStandardItem(temp));

	// chi2 (when applicable)
	temp = QString::fromUtf8("");
	items.append(new QStandardItem(temp));

	mOpenFileModel.appendRow(items);
}

/// Remove the specified data file
///
/// \param index The index of the data one wishes to remove
void wDataEditor::dataRemoved(int index)
{
	if(index < 0)
		return;

	mOpenFileModel.removeRow(index);
}

/// Opens several data files
void wDataEditor::openData(QStringList & filenames)
{
	for(auto filename: filenames)
		mGLWidget->addData(filename.toUtf8().constData());
}

/// Opens an add data dialog when btnAddData is clicked.
void wDataEditor::on_btnAddData_clicked(void)
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
    dialog.setDirectory(mOpenDataDir);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    // Now add in valid file types:
    QStringList filters = mGLWidget->GetFileFilters();
    dialog.setNameFilters(filters);

	if (dialog.exec())
	{
		// store the open data directory
		mOpenDataDir = dialog.directory();
		QStringList files = dialog.selectedFiles();
		openData(files);
	}

	toggleButtons();
}

/// Handle click events on the tree region
void wDataEditor::on_treeOpenFiles_clicked(const QModelIndex & index)
{
	// Columns are: filename, data_type, jd_start, jd_end, jd_mean, chi2r
	int col = index.column();
	if(col == 2 || col == 3 || col == 4)
	{
		QStandardItem *	item = mOpenFileModel.itemFromIndex (index);
		double jd = item->data(Qt::DisplayRole).toDouble();
		if(jd > 0)
			emit(timeSelected(jd));
	}
	else if(col == 5)
	{
		QStandardItem *	item = mOpenFileModel.itemFromIndex (index);
		double wavelength = item->data(Qt::DisplayRole).toDouble();
		if(wavelength > 0)
			emit(wavelengthSelected(wavelength));
	}
}

/// Determines the selected files in the UI and removes them when btnRemoveData
/// is clicked.
void wDataEditor::on_btnRemoveData_clicked()
{
	if(!mGLWidget)
		return;

    // Get the selected indicies:
    QModelIndexList list = this->treeOpenFiles->selectionModel()->selectedRows();

    for(int i = list.size() - 1; i > -1; i--)
    {
    	int id = list[i].row();
    	mGLWidget->removeData(id);
    }

    toggleButtons();
}

/// Refreshes the data displayed in treeOpenFiles
void wDataEditor::refreshTree()
{
	this->treeOpenFiles->setHeaderHidden(false);
	this->treeOpenFiles->setModel(&mOpenFileModel);
	this->treeOpenFiles->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

/// Sets the current widget. Connects necessary signals and slots.
void wDataEditor::setGLWidget(CGLWidget * gl_widget)
{
	mGLWidget = gl_widget;

	// connect any non-automatic signal/slots
	connect(mGLWidget, SIGNAL(dataAdded(CDataInfo)), this, SLOT(addData(CDataInfo)));

	refreshTree();
}

/// Check the status of various UI elements and enable/disable buttons accordingly.
void wDataEditor::toggleButtons()
{
	this->btnAddData->setEnabled(false);
	this->btnRemoveData->setEnabled(false);

	if(!mGLWidget)
		return;

	// Buttons for add/delete data
	this->btnAddData->setEnabled(true);
	if(mOpenFileModel.rowCount() > 0)
		this->btnRemoveData->setEnabled(true);
}
