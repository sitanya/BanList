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

#include <map>
using namespace std;

#pragma once
#ifndef __BanListVAR__
#define __BanListVAR__

void setMASTERGroup(int MasterGroup);
void setMaster(int MasterQQ);
void setQQMSG(char MSG[500]);
void setGroupMSG(char MSG[500]);
void setQQBanMSG(char MSG[500]);
void setGroupBanMSG(char MSG[500]);
map<string, string> getMSG();
void saveMysql();
map<string, bool> getSwitch();
void saveSwitch(map<string,bool>);


#endif /*__BanListVAR__*/
