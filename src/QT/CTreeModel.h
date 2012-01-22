/*
 * CTreeModel.h
 *
 *  Created on: Jan 22, 2012
 *      Author: bkloppenborg
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
