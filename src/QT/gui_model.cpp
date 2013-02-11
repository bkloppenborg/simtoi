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
#include "CModel.h"
#include "CModelFactory.h"
#include "CPositionFactory.h"
#include "CShaderFactory.h"

gui_model::gui_model(QWidget *parent)
    : QDialog(parent)
{
	// setup the UI
	ui.setupUi(this);

	auto models = CModelFactory::Instance();
	auto positions = CPositionFactory::Instance();
	auto shaders = CShaderFactory::Instance();

	SetupComboOptions(ui.cboModels, models.GetModelList());
	SetupComboOptions(ui.cboPositions, positions.GetPositionList());
	SetupComboOptions(ui.cboShaders, shaders.GetShaderList());

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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

	return model;
}
