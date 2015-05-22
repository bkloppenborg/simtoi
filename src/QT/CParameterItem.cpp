/*
 * CParameterItem.cpp
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
 */
 
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

#include "CParameterItem.h"

#include <QtDebug>
#include <QMessageBox>

#include "CModel.h"
#include "CParameter.h"

CParameterItem::CParameterItem(CParameterMap * param_parent, string id)
{
	mParentModel = param_parent;
	mStringID = id;
}

CParameterItem::~CParameterItem()
{
	// TODO Auto-generated destructor stub
}

QVariant CParameterItem::data(int role) const
{
	int col = column();

	QVariant value = QStandardItem::data(role);
	// Modify the number of decimal places displayed for the "value" column
	if(col == 2 && (role == Qt::EditRole || role == Qt::DisplayRole))
	{
		double temp = value.toDouble();
		char format = 'f';

		value = QString("%1").arg(temp, 0, format, decimalPlaces());
	}

	return value;
}

unsigned int CParameterItem::decimalPlaces() const
{
	CParameter & parameter = mParentModel->getParameter(mStringID);
	return parameter.getDecimalPlaces();
}

void CParameterItem::setData(const QVariant & value, int role)
{
	// TODO: This is a really hacky way of setting the parameter values,
	// we should find a different way of doing it?
	int col = column();

	bool exception_thrown = false;

	CParameter & parameter = mParentModel->getParameter(mStringID);

	try
	{
		if(col == 1)
			parameter.setFree( value.toBool() );
		if(col == 2)	// Parameter value
			parameter.setValue( value.toDouble() );
		if(col == 3)	// min value
			parameter.setMin( value.toDouble() );
		if(col == 4)	// max value
			parameter.setMax( value.toDouble() );
		if(col == 5)
			parameter.setStepSize( value.toDouble() );
	}
	catch(range_error & e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString(e.what()));
		msgBox.exec();

		exception_thrown = true;
	}


	if(!exception_thrown)
		QStandardItem::setData(value, role);
}
