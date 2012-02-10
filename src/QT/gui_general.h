/*
 * gui_general.h
 *
 *  Created on: Feb 9, 2012
 *      Author: bkloppenborg
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

	for(int i = 0; i < values.size(); i++)
	{
		tmp_val = QVariant(int(values[i].first));
		tmp_str = QString::fromStdString(values[i].second);
		combo_box->addItem(tmp_str, tmp_val);
	}

	// TODO: Sort the combo box alphabetically.
}

}

#endif /* GUI_GENERAL_H_ */
