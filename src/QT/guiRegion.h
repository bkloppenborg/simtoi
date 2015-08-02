/*
 * guiRegion.h
 *
 *  Created on: Aug 21, 2014
 *      Author: bkloppenborg
 *
 *  A wrapper class for an new model region dialog
 */

#ifndef GUIREGION_H_
#define GUIREGION_H_

#include "ui_guiRegion.h"

class guiRegion : public QDialog, public Ui::guiRegion {
public:
	guiRegion();
	virtual ~guiRegion();
};

#endif /* GUIREGION_H_ */
