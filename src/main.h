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
 * Copyright (c) 2012-2015 Brian Kloppenborg
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <string>
#include <vector>

using namespace std;

string EXE_FOLDER;

int main(int argc, char** argv);
bool ParseArgs(QStringList args, QStringList & filenames, QString & model_file, string &  minimizer, string & output_dir, bool & close_simtoi);
void PrintHelp();

void printFactoryDescription(const vector<string> & ids, const vector<string> & names, const string & title);

#endif /* MAIN_H_ */
