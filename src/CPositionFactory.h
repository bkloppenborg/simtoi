/*
 * CPositionFactory.h
 *
 *  Created on: Feb 10, 2013
 *      Author: bkloppenborg
 */

#ifndef CPOSITIONFACTORY_H_
#define CPOSITIONFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CPosition;

typedef shared_ptr<CPosition> (*CreatePositionFn)(void);


class CPositionFactory {
private:
//	static CPositionFactory * mInstance;

	map<string, CreatePositionFn> mFactory;

	CPositionFactory();

public:
	virtual ~CPositionFactory();

public:
	shared_ptr<CPosition> CreatePosition(string PositionID);

	static CPositionFactory Instance();

	void Register(string PositionID, CreatePositionFn CreateFunction);

	vector<string> GetPositionList();
};

#endif /* CPOSITIONFACTORY_H_ */
