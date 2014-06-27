/*
 * CParameterItem.h
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
 
 /*
 *  This is a really hacky solution to connect the CParameter values to
 *  the QT GUI.  In reality we should probably subclass the QAbstractItemModel
 *  to do the sets/gets instead of this as we duplicate model parameters
 *  and need several signal/slots to get things to work right.
 *
 *  TODO: Find a better way of doing this.
 */

#ifndef CPARAMETERITEM_H_
#define CPARAMETERITEM_H_

#include<QStandardItem>
#include <string>
using namespace std;


class CParameters;
class CParameterMap;

class CParameterItem : public QStandardItem
{
protected:
	CParameters * mParent;
	int mID;

	CParameterMap * mParentModel;
	string mStringID;

public:
	CParameterItem(CParameters * item_parent, int id);
	CParameterItem(CParameterMap * param_parent, string name);
	virtual ~CParameterItem();

	QVariant data(int role = Qt::UserRole + 1) const;
	void setData(const QVariant & value, int role = Qt::UserRole + 1);


};

#endif /* CPARAMETERITEM_H_ */
