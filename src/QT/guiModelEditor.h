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
 * Copyright (c) 2014 Brian Kloppenborg
 */

#ifndef GUIMODEL_H_
#define GUIMODEL_H_

#include <QDialog>
#include <memory>

#include "ui_guiModelEditor.h"

using namespace std;

class CModel;
typedef shared_ptr<CModel> CModelPtr;

class guiModelEditor : public QDialog, public Ui::guiModelEditor {
	Q_OBJECT

protected:
	CModelPtr mModel;

public:
	guiModelEditor();
	guiModelEditor(CModelPtr model);
	virtual ~guiModelEditor();

	CModelPtr getModel();

protected:
	void initUi();

public slots:
	void on_btnFeatureAdd_clicked();
	void on_btnFeatureRemove_clicked();
};

#endif /* GUIMODEL_H_ */
