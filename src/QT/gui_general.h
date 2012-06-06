/*
 * gui_general.h
 *
 *  Created on: Feb 9, 2012
 *      Author: bkloppenborg
 */

/* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
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
 */

#ifndef GUI_GENERAL_H_
#define GUI_GENERAL_H_

namespace gui_general
{

template <typename T>
void SetupComboOptions(QComboBox * combo_box, vector< pair<T, string> > values)
{
	QString tmp_str = "";
	QVariant tmp_val = "";

	for(unsigned int i = 0; i < values.size(); i++)
	{
		tmp_val = QVariant(int(values[i].first));
		tmp_str = QString::fromStdString(values[i].second);
		combo_box->addItem(tmp_str, tmp_val);
	}

	// TODO: Sort the combo box alphabetically.
}

}

#endif /* GUI_GENERAL_H_ */
