/*
 * CData.h
 *
 *  Created on: May 12, 2015
 *      Author: bkloppenborg
 *
 *  A base class for a data containers
 */

#ifndef SRC_CDATA_H_
#define SRC_CDATA_H_

#include <vector>
#include <string>
#include <memory>

using namespace std;

#include "CDataInfo.h"

class CDataHandler;
typedef shared_ptr<CDataHandler> CDataHandlerPtr;

class CData : public CDataInfo {
protected:
	CDataHandlerPtr mHandler;

public:
	CData();
	virtual ~CData();

	CDataHandlerPtr handler() { return mHandler; };

	virtual CDataInfo open(const string & filename) = 0;
	virtual void saveAsText(const string & directory) = 0;
//	virtual void save(const string & filename) = 0;

	virtual unsigned int size() = 0;

	static string stripPath(const string & filename);
	static string stripExtension(const string & filename, vector<string> & valid_extensions);

};

#endif /* SRC_CDATA_H_ */
