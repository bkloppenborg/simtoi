/*
 * guiModel.h
 *
 *  Created on: Aug 21, 2014
 *      Author: bkloppenborg
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
