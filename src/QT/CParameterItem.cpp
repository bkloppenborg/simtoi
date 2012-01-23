/*
 * CParameterItem.cpp
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
 */

#include "CParameterItem.h"

#include <QtDebug>

#include "CParameters.h"

CParameterItem::CParameterItem(CParameters * parent, int id)
{
	mParent = parent;
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
	// TODO: This is really hacky, and possibly can fail if a parameter is
	// of an integer type!
	if(role ==  Qt::EditRole)
	{
		if(value.type() == QVariant::Int)
			mParent->SetFree(mID, value.toBool());
		else if(value.type() == QVariant::Double)
			mParent->SetParam(mID, float(value.toDouble()));
	}

	QStandardItem::setData(value, role);
}
