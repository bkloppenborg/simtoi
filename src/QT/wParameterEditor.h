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

#ifndef WPARAMETER_EDITOR_H_
#define WPARAMETER_EDITOR_H_

#include <QWidget>
#include <memory>
#include <QList>
#include <QStandardItem>

#include "ui_wParameterEditor.h"
#include "CGLWidget.h"
#include "CParameterItem.h"
#include "CParameterMap.h"
#include "CTreeModel.h"

using namespace std;

class wParameterEditor : public QWidget, public Ui::wParameterEditor {
	Q_OBJECT

protected:
	CGLWidget * mGLWidget;
    CTreeModel mTreeModel;

public:
    wParameterEditor(QWidget * parent = 0);
	virtual ~wParameterEditor();

	void setGLWidget(CGLWidget * gl_widget);

public:
	void buildTree();
	void toggleButtons();
	int getSelectedModelIndex();
	void refreshTree();

    static void LoadParameters(QStandardItem * parent_widget, CParameterMap * param_map);
    static QList<QStandardItem *> LoadParametersHeader(QString name, CParameterMap * param_map);

private slots:
    void on_btnAddModel_clicked();
    void on_btnEditModel_clicked();
    void on_btnRemoveModel_clicked();

public slots:
    void updateModels();
};

#endif /* WPARAMETER_EDITOR_H_ */
