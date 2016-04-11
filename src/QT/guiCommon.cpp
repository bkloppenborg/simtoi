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
 * Copyright (c) 2014 Brian Kloppenborg
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
