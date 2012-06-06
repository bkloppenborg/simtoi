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
#include "gui_general.h"

gui_model::gui_model(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	mModelType = CModelList::NONE;
	mShaderType = CGLShaderList::NONE;

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(ValuesAccepted()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

gui_model::~gui_model()
{

}

void gui_model::SetFeatureTypes(vector< pair<int, string> > feature_info)
{
//	SetupComboOptions(ui.listFeatures, feature_info);
}

void gui_model::SetModelTypes(vector< pair<CModelList::ModelTypes, string> > model_info)
{
	gui_general::SetupComboOptions(ui.cboModels, model_info);
}

void gui_model::SetPositionTypes(vector< pair<CPosition::PositionTypes, string> > position_info)
{
	gui_general::SetupComboOptions(ui.cboPositions, position_info);
}

void gui_model::SetShaderTypes(vector< pair<CGLShaderList::ShaderTypes, string> > shader_info)
{
	gui_general::SetupComboOptions(ui.cboShaders, shader_info);
}

void gui_model::ValuesAccepted(void)
{
	// First pull out the model
	int value = ui.cboModels->itemData(ui.cboModels->currentIndex()).toInt();
	if(value > CModelList::NONE && value < CModelList::LAST_VALUE)
		mModelType = CModelList::ModelTypes(value);

	// Position
	value = ui.cboPositions->itemData(ui.cboPositions->currentIndex()).toInt();
	if(value > CPosition::NONE && value < CPosition::LAST_VALUE)
		mPositionType = CPosition::PositionTypes(value);

	// Shader
	value = ui.cboShaders->itemData(ui.cboShaders->currentIndex()).toInt();
	if(value > CGLShaderList::NONE && value < CGLShaderList::LAST_VALUE)
		mShaderType = CGLShaderList::ShaderTypes(value);


	// Now call the default close function.
	done(1);
}
