/*
 * CParameterItem.h
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
 *
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

class CParameters;

class CParameterItem : public QStandardItem
{
protected:
	CParameters * mParent;
	int mID;

public:
	CParameterItem(CParameters * parent, int id);
	virtual ~CParameterItem();

	QVariant data(int role = Qt::UserRole + 1) const;
	void setData(const QVariant & value, int role = Qt::UserRole + 1);


};

#endif /* CPARAMETERITEM_H_ */
