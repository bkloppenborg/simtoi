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

#ifndef LOAD_MODELS_H_
#define LOAD_MODELS_H_

#include "CModelFactory.h"

namespace models
{

// Call this function to load all minimization engines.
void load();

}

#endif // LOAD_MODELS_H_