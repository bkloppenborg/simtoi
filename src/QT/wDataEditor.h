 /*
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
 *
 * Copyright (c) 2015 Brian Kloppenborg
 */

#ifndef WDATA_EDITOR_H_
#define WDATA_EDITOR_H_

#include "ui_wDataEditor.h"
#include <QWidget>
#include <QDir>
#include <QStandardItemModel>
#include <QStringList>
#include <memory>

#include "CGLWidget.h"

using namespace std;

class CDataInfo;

class wDataEditor : public QWidget, public Ui::wDataEditor {
	Q_OBJECT

protected:
	CGLWidget * mGLWidget;
	QDir mOpenDataDir;
    QStandardItemModel mOpenFileModel;

public:
	wDataEditor(QWidget * parent = 0);
	virtual ~wDataEditor();

	void setGLWidget(CGLWidget * gl_widget);

	void openData(QStringList & filenames);

	void toggleButtons();
	void refreshTree();

signals:
	void timeSelected(double JD);
	void wavelengthSelected(double wavelength);

private slots:

	void on_btnAddData_clicked();
	void on_btnRemoveData_clicked();
	void on_treeOpenFiles_clicked(const QModelIndex & index);

	void addData(const CDataInfo & data_info);
	void dataRemoved(int index);
};

#endif /* WDATA_EDITOR_H_ */
