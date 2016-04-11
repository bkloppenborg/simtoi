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
 * Copyright (c) 2012 Brian Kloppenborg
 */

#include "CTreeModel.h"

CTreeModel::CTreeModel()
{
	// TODO Auto-generated constructor stub

}

CTreeModel::~CTreeModel()
{
	// TODO Auto-generated destructor stub
}

bool CTreeModel::setData(const QModelIndex & item_index, const QVariant & value, int role)
{
	bool tmp = QStandardItemModel::setData(item_index, value, role);
	emit parameterUpdated();

	return tmp;
}
