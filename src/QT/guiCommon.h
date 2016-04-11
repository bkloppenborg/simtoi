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
void setOptions(QComboBox * comboBox, const vector<string> & options);

void setSelectedValue(QComboBox * comboBox, const QString & value);

void setSelectedValue(QComboBox * comboBox, const QVariant & value);

}	// namespace guiCommon


#endif /* GUICOMMON_H_ */
