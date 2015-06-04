/*
 * \file CMinimizerFactory.h
 *
 *  Created on: Feb 10, 2013
 *  \author bkloppenborg
 */

 /*
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Minimizering
 * Tool for Optical Interferometry" (Version X).
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Minimizering Tool for Optical
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

#ifndef CMINIMIZERFACTORY_H_
#define CMINIMIZERFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CMinimizerThread;
typedef shared_ptr<CMinimizerThread> CMinimizerPtr;
typedef CMinimizerPtr (*CreateMinimizerFn)(void);

/// \brief A factory class for creating minimizer objects
///
/// This factory is used to register and create instances of minimizers in SIMTOI.
/// As this class is a singleton, the constructor is private. Call the `Instance()`
/// function to get a copy of this object.
///
/// Before any minimizer may be used in SIMTOI, it must be registered with this class.
/// Simply call `Register()` with a unique ID and `CMinimizerPtr` creation function.
/// After the minimizer has been registered it will become available for use in SIMTOI.
class CMinimizerFactory {
private:
	map<string, CreateMinimizerFn> mCreators; ///< Associates id -> minimizer create function
	map<string, string> mNames; ///< Associates id -> minimizer create function

	CMinimizerFactory();	///< Singleton class => private constructor.

public:
	virtual ~CMinimizerFactory();

public:
	shared_ptr<CMinimizerThread> create(string ID);

	string idFromName(string name);

	static CMinimizerFactory getInstance();
	void addItem(CreateMinimizerFn CreateFunction);

	vector<string> getIDs();
	vector<string> getNames();
};

#endif /* CMINIMIZERFACTORY_H_ */
