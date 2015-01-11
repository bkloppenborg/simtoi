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
void setOptions(QComboBox * comboBox, const vector<string> & options)
{
	QString tmp_str;
	for(auto str : options)
	{
		tmp_str = QString::fromStdString(str);
		comboBox->addItem(tmp_str);
	}
}

/// Sets the combo box to the specified value if found.
void setSelectedValue(QComboBox * comboBox, const QString & value)
{
	int index = comboBox->findText(value);
	if ( index != -1 ) { // -1 for not found
		comboBox->setCurrentIndex(index);
	}
}

/// Sets the combo box to the specified value if found.
void setSelectedValue(QComboBox * comboBox, const QVariant & value)
{
	int index = comboBox->findData(value);
	if ( index != -1 ) { // -1 for not found
		comboBox->setCurrentIndex(index);
	}
}

}	// namespace guiCommon


#endif /* GUICOMMON_H_ */
