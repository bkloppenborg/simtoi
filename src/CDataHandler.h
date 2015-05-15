/*
 * CDataHandler.h
 *
 *  Created on: May 12, 2015
 *      Author: bkloppenborg
 */

#ifndef SRC_CDATAHANDLER_H_
#define SRC_CDATAHANDLER_H_

#include <string>
#include <vector>
#include <memory>

using namespace std;

class CData;
typedef shared_ptr<CData> CDataPtr;

class CDataHandler {

protected:
	string mDataDescription; 	///< A short few-word phrase to describe the data.
	vector<string> mExtensions; ///< A list of valid extensions for this task.

public:
	CDataHandler();
	virtual ~CDataHandler();

public:

	virtual void getChi(CDataPtr data, double * chis, unsigned int size) = 0;
	virtual void getUncertainties(CDataPtr data, double * residuals, unsigned int size) = 0;

	string description() const { return mDataDescription; };
	vector<string> fileExtensions() const { return mExtensions; };
};

#endif /* SRC_CDATAHANDLER_H_ */
