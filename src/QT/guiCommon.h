/*
 * guiCommon.h
 *
 *  Created on: Aug 29, 2014
 *      Author: bkloppenborg
 */

#ifndef GUICOMMON_H_
#define GUICOMMON_H_

#include <QComboBox>
#include <QString>
#include <vector>
#include <string>

using namespace std;

namespace guiCommon
{

/// Populates a QComboBox with string stored in options
void setOptions(QComboBox * comboBox, const vector<string> & options);

void setSelectedValue(QComboBox * comboBox, const QString & value);

void setSelectedValue(QComboBox * comboBox, const QVariant & value);

}	// namespace guiCommon


#endif /* GUICOMMON_H_ */
