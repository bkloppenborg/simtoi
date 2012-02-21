/*
 * CVectorList.h
 *
 *  Created on: Jan 21, 2012
 *      Author: bkloppenborg
 */

#ifndef CVECTORLIST_H_
#define CVECTORLIST_H_

#include <vector>
using namespace std;

template <class T>

class CVectorList
{
protected:
	vector<T> mList;

public:
	CVectorList() {};

	~CVectorList()
	{
		Clear();
	}

	void Append(T item)
	{
		mList.push_back(item);
	}

	void Clear()
	{
		for(int i = mList.size() - 1; i > -1; i--)
			delete mList[i];
	}

	void Remove(int i)
	{
		if(i < mList.size())
		{
			delete mList[i];
			mList.erase(mList.begin() + i);
		}
	}

	int size(void)
	{
		return mList.size();
	}

	T operator[](int i) { return mList[i]; }
};

#endif /* CVECTORLIST_H_ */
