/*
 * CTreeModel.h
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

#ifndef CTREEMODEL_H_
#define CTREEMODEL_H_

#include <QObject>
#include <QStandardItemModel>

class CTreeModel : public QStandardItemModel
{
	Q_OBJECT

public:
	CTreeModel();
	virtual ~CTreeModel();

	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

	signals:
	void parameterUpdated(void);
};

#endif /* CTREEMODEL_H_ */
