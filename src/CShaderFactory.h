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
 * Copyright (c) 2013 Brian Kloppenborg
 */

#ifndef CSHADERFACTORY_H_
#define CSHADERFACTORY_H_

#include <map>
#include <memory>
#include <vector>

using namespace std;

class CShader;

typedef shared_ptr<CShader> CShaderPtr;

class CShaderFactory {
private:

	map<string, CShaderPtr> mFactory;

	CShaderFactory();

public:
	virtual ~CShaderFactory();

public:
	shared_ptr<CShader> CreateShader(string GLShaderID);

	static CShaderFactory Instance();

	void Register(string json_config_file);
	void Register(string shader_id, CShaderPtr shader);

	vector<string> GetShaderList();
};

#endif /* CSHADERFACTORY_H_ */
