/*
* Dice! QQ Dice Robot for TRPG
* Copyright (C) 2018 w4123ËÝä§
*
* This program is free software: you can redistribute it and/or modify it under the terms
* of the GNU Affero General Public License as published by the Free Software Foundation,
* either version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License along with this
* program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#ifndef __MYSQlVAR__
#define __MYSQlVAR__

#include <string>
#include <set>
#include <map>

using namespace std;

set<long long> QueryBlack(bool);
bool InsertBlack(long long,bool);
bool ModifyData();
bool DeleteBlack(long long,bool);
map<string, string> QueryMsg(long long);
bool InsertMsg(map<string, string>, long long);
bool initMsg(long long);

#endif /*__MYSQlVAR__*/
