/*
 * CTreeModel.cpp
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
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

bool CTreeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	bool tmp = QStandardItemModel::setData(index, value, role);
	emit parameterUpdated();

	return tmp;
}
