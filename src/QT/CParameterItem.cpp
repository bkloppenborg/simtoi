/*
 * CParameterItem.cpp
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
 */

#include "CParameterItem.h"

#include <QtDebug>

#include "CParameters.h"

CParameterItem::CParameterItem(CParameters * item_parent, int id)
{
	mParent = item_parent;
	mID = id;

}

CParameterItem::~CParameterItem()
{
	// TODO Auto-generated destructor stub
}

QVariant CParameterItem::data(int role) const
{
	return QStandardItem::data(role);
}

void CParameterItem::setData(const QVariant & value, int role)
{
	// TODO: This is a really hacky way of setting the parameter values,
	// we should find a different way of doing it?
	int col = column();

	if(role == Qt::CheckStateRole || role == Qt::EditRole)
	{
		if(col == 1)
			mParent->SetFree(mID, value.toBool());
		if(col == 2)	// Parameter value
			mParent->SetParam(mID, float(value.toDouble()));
		if(col == 3)	// min value
			mParent->SetMin(mID, float(value.toDouble()));
		if(col == 4)	// max value
			mParent->SetMax(mID, float(value.toDouble()));
	}

	QStandardItem::setData(value, role);
}
