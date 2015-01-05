/*
 * CDataInfo.h
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
 */

#ifndef SRC_CDATAINFO_H_
#define SRC_CDATAINFO_H_

#include <string>

using namespace std;

class CDataInfo {
public:
	CDataInfo();
	virtual ~CDataInfo();

public:
	string mFilename;
	string mQuantityDescription;
	double mJDStart;
	double mJDEnd;
	double mJDMean;

public:
	string getFilename() { return mFilename; };
	string getQuantityDescription() { return mQuantityDescription; };

};

#endif /* SRC_CDATAINFO_H_ */
