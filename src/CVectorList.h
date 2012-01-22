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
		for(int i = mList.size() - 1; i > -1; i--)
			delete mList[i];
	}

	void Append(T item)
	{
		mList.push_back(item);
	}

	void GetParameters(float * params, int n_params)
	{
	    int n = 0;

	    // Now call render on all of the models:
	    for(typename vector<T>::iterator it = mList.begin(); it != mList.end(); ++it)
	    {
	    	(*it)->GetAllParameters(params + n, n_params - n);
	    	n += (*it)->GetTotalFreeParameters();
	    }
	}

	void SetParameters(float * params, int n_params)
	{
	    int n = 0;

	    // Now call render on all of the models:
	    for(typename vector<T>::iterator it = mList.begin(); it != mList.end(); ++it)
	    {
	    	(*it)->SetAllParameters(params + n, n_params - n);
	    	n += (*it)->GetTotalFreeParameters();
	    }
	}
};

#endif /* CVECTORLIST_H_ */
