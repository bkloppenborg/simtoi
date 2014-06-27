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

#include "gui_model.h"
#include "gui_common.h"
#include "CModel.h"
#include "CModelFactory.h"
#include "CPositionFactory.h"
#include "CShaderFactory.h"
#include "CFeatureFactory.h"

gui_model::gui_model(QWidget *parent)
    : QDialog(parent)
{
	SetupUI();

}

gui_model::gui_model(QWidget *parent, shared_ptr<CModel> model)
{
	SetupUI();

	// Find the model, position, and shader IDs from the combo boxes:
	int model_id = ui.cboModels->findText(QString::fromStdString(model->getID()));
	int position_id = ui.cboPositions->findText(QString::fromStdString(model->GetPosition()->GetID()));
	int shader_id = ui.cboShaders->findText(QString::fromStdString(model->GetShader()->GetID()));

	// Set the current selected text based upon the model IDs
	ui.cboModels->setCurrentIndex(model_id);
	ui.cboPositions->setCurrentIndex(position_id);
	ui.cboShaders->setCurrentIndex(shader_id);
}

gui_model::~gui_model()
{

}

/// Returns a copy of the selected model.
shared_ptr<CModel> gui_model::GetModel(void)
{
	auto factory = CModelFactory::Instance();

	string model_id = ui.cboModels->currentText().toStdString();
	shared_ptr<CModel> model = factory.CreateModel(model_id);

	string position_id = ui.cboPositions->currentText().toStdString();
	model->SetPositionModel(position_id);

	string shader_id = ui.cboShaders->currentText().toStdString();
	model->SetShader(shader_id);

	QListWidgetItem * item;
	string feature_id;
	for ( int i = 0 ; i < ui.listFeatures->count() ; ++i )
	{
		item = ui.listFeatures->item(i);
		feature_id = item->data(Qt::DisplayRole).toString().toStdString();
		model->AddFeature(feature_id);
	}

	return model;
}

void gui_model::SetupUI()
{
	// setup the UI
	ui.setupUi(this);

	auto models = CModelFactory::Instance();
	auto positions = CPositionFactory::Instance();
	auto shaders = CShaderFactory::Instance();
	auto features = CFeatureFactory::Instance();

	gui_common::SetupOptions(ui.cboModels, models.GetModelList());
	gui_common::SetupOptions(ui.cboPositions, positions.GetPositionList());
	gui_common::SetupOptions(ui.cboShaders, shaders.GetShaderList());
	gui_common::SetupOptions(ui.cboFeatures, features.GetFeatureList());

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void gui_model::on_btnFeatureAdd_clicked()
{
	QString feature_id = ui.cboFeatures->currentText();
	ui.listFeatures->addItem(feature_id);
}

void gui_model::on_btnFeatureRemove_clicked()
{
	qDeleteAll(ui.listFeatures->selectedItems());
}
