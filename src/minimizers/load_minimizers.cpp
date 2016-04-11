/*
 * This file is part of the SImulation and Modeling Tool for Optical
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
 *
 *  Copyright (c) 2015 Brian Kloppenborg
 */

#include "load_minimizers.h"

#include "minimizers/CBenchmark.h"
#include "minimizers/CGridSearch.h"
// Compiler directive to add support for levmar
#ifdef _ADD_LEVMAR
#include "minimizers/CLevmar.h"
#include "minimizers/CBootstrap_Levmar.h"
#endif //_ADD_LEVMAR
// Compiler directive to add support for MultiNest
#ifdef _ADD_MULTINEST
#include "minimizers/CMultiNest.h"
#endif // _ADD_MULTINEST
#ifdef _ADD_NLOPT
#include "minimizers/CNLopt.h"
#endif // _ADD_NLOPT

namespace minimizers {

void load()
{

	// TODO: For now we CMinimizerFactory::getInstance().addItem minimizers explicitly. In the future, we should use plugins instead.
	CMinimizerFactory::getInstance().addItem(&CBenchmark::Create);
	CMinimizerFactory::getInstance().addItem(&CGridSearch::Create);

	// NLOpt engines
#ifdef _ADD_NLOPT
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateNELDERMEAD);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateDIRECTL);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateDIRECT);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateCRS2);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateMLSLLDS);
	//  CMinimizerFactory::getInstance().addItem("&CNLopt::CreateSTOGORAND); // missing from Ubuntu's NLOpt library, disable
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateISRES);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateESCH);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateCOBYLA);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateBOBYQA);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateNEWUOA);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreatePRAXIS);
	CMinimizerFactory::getInstance().addItem(&CNLopt::CreateSBPLX);
#endif //_ADD_NLOPT

	// Levmar
#ifdef _ADD_LEVMAR
	CMinimizerFactory::getInstance().addItem(&CLevmar::Create);
	CMinimizerFactory::getInstance().addItem(&CBootstrap_Levmar::Create);
#endif //_ADD_LEVMAR

	// MultiNest
#ifdef _ADD_MULTINEST
	CMinimizerFactory::getInstance().addItem(&CMultiNest::Create);
#endif // _ADD_MULTINEST
}

} // namespace minimizers
