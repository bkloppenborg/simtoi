/*
 * CModelFactory.h
 *
 *  Created on: Feb 10, 2013
 *      Author: bkloppenborg
 */

#ifndef CMODELFACTORY_H_
#define CMODELFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CModel;

typedef shared_ptr<CModel> (*CreateModelFn)(void);


class CModelFactory {
private:
//	static CModelFactory * mInstance;

	map<string, CreateModelFn> mFactory;

	CModelFactory();

public:
	virtual ~CModelFactory();

public:
	shared_ptr<CModel> CreateModel(string ModelID);

	static CModelFactory Instance();

	void Register(string ModelID, CreateModelFn CreateFunction);

	vector<string> GetModelList();
};

#endif /* CMODELFACTORY_H_ */
