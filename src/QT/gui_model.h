
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
 
#ifndef GUI_ADDMODEL_H
#define GUI_ADDMODEL_H


#include <QtGui/QWidget>
#include "ui_gui_model.h"
#include <QDialog>
#include <utility>
#include <vector>
#include "CModelList.h"
#include "CGLShaderList.h"
#include "CModelList.h"

using namespace std;

class gui_model : public QDialog
{
    Q_OBJECT
private:
    Ui::gui_addmodelClass ui;

protected:
    CGLShaderList::ShaderTypes mShaderType;
    CModelList::ModelTypes mModelType;
    CPosition::PositionTypes mPositionType;

public:
    gui_model(QWidget *parent = 0);
    ~gui_model();

    CModelList::ModelTypes GetModelType() { return mModelType; };
    CGLShaderList::ShaderTypes GetShaderType() { return mShaderType; };
    CPosition::PositionTypes GetPositionType() { return mPositionType; };


    void SetFeatureTypes(vector< pair<int, string> > feature_info);
    void SetModelTypes(vector< pair<CModelList::ModelTypes, string> > model_info);
    void SetPositionTypes(vector< pair<CPosition::PositionTypes, string> > position_info);
    void SetShaderTypes(vector< pair<CGLShaderList::ShaderTypes, string> > shader_info);

    public slots:
    void ValuesAccepted();

};

#endif // GUI_ADDMODEL_H
