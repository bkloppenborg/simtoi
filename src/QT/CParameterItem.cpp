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
	switch(role)
	{
	case Qt::CheckStateRole:
		// Checkbox enabled/disabled
		if(value.type() == QVariant::Int)
			mParent->SetFree(mID, value.toBool());
		break;

	case Qt::EditRole:
		if(value.type() == QVariant::Double)
			mParent->SetParam(mID, float(value.toDouble()));
		break;

	}

	QStandardItem::setData(value, role);
}
