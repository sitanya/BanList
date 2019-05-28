/*
* Dice! QQ Dice Robot for TRPG
* Copyright (C) 2018 w4123���
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
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>
#include "mysql.h"
#include "MysqlClient.h"
#include "winmain.h"
#include "CQAPI.h"
#include "stdafx.h"

#include "APPINFO.h"
#include "CQEVE_ALL.h"
#include "CQTools.h"
#include "GlobalVar.h"
#include "DiceMsgSend.h"
#include "NameStorage.h"
#include "BanList.h"

using namespace std;
using namespace CQ;
int MASTER = 450609203;
int MASTERGroup = 112380103;
map<string, string> Messages;
map<string, bool> Switch;

unique_ptr<NameStorage> Name;

inline void init(string &msg)
{
	msg_decode(msg);
}

inline void init2(string &msg)
{
	for (int i = 0; i != msg.length(); i++)
	{
		if (msg[i] < 0)
		{
			if ((msg[i] & 0xff) == 0xa1 && (msg[i + 1] & 0xff) == 0xa1)
			{
				msg[i] = 0x20;
				msg.erase(msg.begin() + i + 1);
			}
			else if ((msg[i] & 0xff) == 0xa3 && (msg[i + 1] & 0xff) >= 0xa1 && (msg[i + 1] & 0xff) <= 0xfe)
			{
				msg[i] = msg[i + 1] - 0x80;
				msg.erase(msg.begin() + i + 1);
			}
			else
			{
				i++;
			}
		}
	}

	while (isspace(msg[0]))
		msg.erase(msg.begin());
	while (!msg.empty() && isspace(msg[msg.length() - 1]))
		msg.erase(msg.end() - 1);
	if (msg.substr(0, 2) == "��")
	{
		msg.erase(msg.begin());
		msg[0] = '.';
	}
	if (msg[0] == '!')
		msg[0] = '.';
}

vector<string> split(string &str, const char *c)
{
	char *cstr, *p;
	vector<string> res;
	cstr = new char[str.size() + 1];
	strcpy(cstr, str.c_str());
	p = strtok(cstr, c);
	while (p != NULL)
	{
		res.push_back(p);
		p = strtok(NULL, c);
	}
	return res;
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	if ("" == str)
		return res;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����
	char *strs = new char[str.length() + 1]; //��Ҫ����
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;	 //�ָ�õ����ַ���ת��Ϊstring����
		res.push_back(s); //����������
		p = strtok(NULL, d);
	}

	return res;
}

std::string strip(std::string origin)
{
	bool flag = true;
	while (flag)
	{
		flag = false;
		if (origin[0] == '!' || origin[0] == '.')
		{
			origin.erase(origin.begin());
			flag = true;
		}
		else if (origin.substr(0, 2) == "��" || origin.substr(0, 2) == "��")
		{
			origin.erase(origin.begin());
			origin.erase(origin.begin());
			flag = true;
		}
	}
	return origin;
}

std::string getName(long long QQ, long long GroupID = 0)
{
	if (GroupID)
	{
		/*Ⱥ*/
		if (GroupID < 1000000000)
		{
			return strip(Name->get(GroupID, QQ).empty()
							 ? (getGroupMemberInfo(GroupID, QQ).GroupNick.empty()
									? getStrangerInfo(QQ).nick
									: getGroupMemberInfo(GroupID, QQ).GroupNick)
							 : Name->get(GroupID, QQ));
		}
		/*������*/
		return strip(Name->get(GroupID, QQ).empty() ? getStrangerInfo(QQ).nick : Name->get(GroupID, QQ));
	}
	/*˽��*/
	return strip(getStrangerInfo(QQ).nick);
}

void setMaster(int Master)
{
	MASTER = Master;
}

void setMASTERGroup(int MasterGroup)
{
	MASTERGroup = MasterGroup;
}

void setQQMSG(char Msg[500])
{
	string tmp = Msg;
	vector<string> tmp2 = split(tmp, "\\n");
	string tmp3 = "";
	for (int i = 0; i < tmp2.size(); i++)
	{
		tmp3 += tmp2[i] + "\n";
	}
	Messages["QQMsg"] = tmp3.substr(0, tmp3.length() - 1);
}

void setGroupMSG(char Msg[500])
{
	string tmp = Msg;
	vector<string> tmp2 = split(tmp, "\\n");
	string tmp3 = "";
	for (int i = 0; i < tmp2.size(); i++)
	{
		tmp3 += tmp2[i] + "\n";
	}
	Messages["GroupMsg"] = tmp3.substr(0, tmp3.length() - 1);
}

void setQQBanMSG(char Msg[500])
{
	string tmp = Msg;
	vector<string> tmp2 = split(tmp, "\\n");
	string tmp3 = "";
	for (int i = 0; i < tmp2.size(); i++)
	{
		tmp3 += tmp2[i] + "\n";
	}
	Messages["BanQQMsg"] = tmp3.substr(0, tmp3.length() - 1);
}

void setGroupBanMSG(char Msg[500])
{
	string tmp = Msg;
	vector<string> tmp2 = split(tmp, "\\n");
	string tmp3 = "";
	for (int i = 0; i < tmp2.size(); i++)
	{
		tmp3 += tmp2[i] + "\n";
	}
	Messages["BanGroupMsg"] = tmp3.substr(0, tmp3.length() - 1);
}

map<string, string> getMSG()
{
	map<string, string> Messages_tmp = QueryMsg(getLoginQQ());
	if (!Messages_tmp.empty())
	{
		Messages = Messages_tmp;
		MASTER = stoi(Messages_tmp["MASTER"]);
		MASTERGroup = stoi(Messages_tmp["MASTERGroup"]);

		return Messages_tmp;
	}
	else
	{
		Messages["MASTER"] = to_string(MASTER);
		Messages["MASTERGroup"] = to_string(MASTERGroup);
		return Messages;
	}
}

void saveMysql()
{
	Messages["MASTER"] = to_string(MASTER);
	Messages["MASTERGroup"] = to_string(MASTERGroup);
	InsertMsg(Messages, getLoginQQ());
}

void showswitch() {
	map<string, bool> switch_tmp_value=getSwitch();
	AddMsgToQueue("LeaveBanGroup"+to_string(switch_tmp_value["LeaveBanGroup"]), 450609203);
	AddMsgToQueue("refuseGroup" + to_string(switch_tmp_value["refuseGroup"]), 450609203);
	AddMsgToQueue("BanDetachOperator" + to_string(switch_tmp_value["BanDetachOperator"]), 450609203);
	AddMsgToQueue("deleteDetachOperator" + to_string(switch_tmp_value["deleteDetachOperator"]), 450609203);
	AddMsgToQueue("agreeGroup" + to_string(switch_tmp_value["agreeGroup"]), 450609203);
	AddMsgToQueue("LeaveForbiddenGroup" + to_string(switch_tmp_value["LeaveForbiddenGroup"]), 450609203);
	AddMsgToQueue("BanForbiddenGroup" + to_string(switch_tmp_value["BanForbiddenGroup"]), 450609203);
	AddMsgToQueue("DontRunningInBanGroup" + to_string(switch_tmp_value["DontRunningInBanGroup"]), 450609203);
	AddMsgToQueue("BanDetachGroup" + to_string(switch_tmp_value["BanDetachGroup"]), 450609203);
	AddMsgToQueue("LeaveGroupByUser" + to_string(switch_tmp_value["LeaveGroupByUser"]), 450609203);
	AddMsgToQueue("DontRunningInBanGroupForUser" + to_string(switch_tmp_value["DontRunningInBanGroupForUser"]), 450609203);
	AddMsgToQueue("refuseBanUser" + to_string(switch_tmp_value["refuseBanUser"]), 450609203);
	AddMsgToQueue("agreeUser" + to_string(switch_tmp_value["agreeUser"]), 450609203);
}

map<string, bool> getSwitch()
{
	map<string, bool> Switch_tmp = QuerySwitch(getLoginQQ());
	if (!Switch_tmp.empty())
	{
		Switch = Switch_tmp;
		return Switch;
	}
	else
	{
		Switch_tmp["LeaveBanGroup"] = false;
		Switch_tmp["refuseGroup"] = false;
		Switch_tmp["BanDetachOperator"] = false;
		Switch_tmp["deleteDetachOperator"] = false;
		Switch_tmp["agreeGroup"] = false;
		Switch_tmp["LeaveForbiddenGroup"] = false;
		Switch_tmp["BanForbiddenGroup"] = false;
		Switch_tmp["DontRunningInBanGroup"] = false;
		Switch_tmp["BanDetachGroup"] = false;

		Switch_tmp["LeaveGroupByUser"] = false;
		Switch_tmp["DontRunningInBanGroupForUser"] = false;
		Switch_tmp["refuseBanUser"] = false;
		Switch_tmp["agreeUser"] = false;
		return Switch_tmp;
	}
}

void saveSwitch(map<string, bool> switch_tmp)
{
	InsertSwitch(switch_tmp, getLoginQQ());
	Switch=getSwitch();
}

struct SourceType
{
	SourceType(long long a, int b, long long c) : QQ(a), Type(b), GrouporDiscussID(c) {};
	long long QQ = 0;
	int Type = 0;
	long long GrouporDiscussID = 0;
	bool operator<(SourceType b) const
	{
		return this->QQ < b.QQ;
	}
};

using PropType = map<string, int>;
EVE_Enable(__eventEnable)
{
	initMsg(getLoginQQ());
	initSwitch(getLoginQQ());
	Switch = getSwitch();
	Messages = getMSG();
	//Wait until the thread terminates
	while (msgSendThreadRunning)
		Sleep(10);

	thread msgSendThread(SendMsg);
	msgSendThread.detach();
	return 0;
}

EVE_PrivateMsg_EX(__eventPrivateMsg)
{
	if (eve.isSystem())
		return;
	init(eve.message);
	init2(eve.message);
	if (eve.message[0] != '.')
		return;
	int intMsgCnt = 1;
	while (isspace(eve.message[intMsgCnt]))
		intMsgCnt++;
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	set<long long> BanedQQList;
	BanedQQList = QueryBlack(false);
	if (BanedQQList.count(eve.fromQQ) && eve.fromQQ != MASTER)
	{
		AddMsgToQueue("����˽����Ϣ�ѱ����أ���Ϊ�����ں�������", eve.fromQQ);
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "exit")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 4;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strgroup = "";
		if (strgroup == "")
		{
			AddMsgToQueue("��������Ҫ�˳���Ⱥ�ţ�", eve.fromQQ);
			return;
		}
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strgroup += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strgroup)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		const long long llgroup = stoll(strgroup);
		if (setGroupLeave(llgroup) == 0)
			AddMsgToQueue("�Ѵ�" + strgroup + "���˳�", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "ban" && strLowerMessage.substr(intMsgCnt, 7) != "banlist" && strLowerMessage.substr(intMsgCnt, 8) != "bangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 3;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (strLowerMessage[intMsgCnt])
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "")
		{
			AddMsgToQueue("��������QQ�ţ�", eve.fromQQ);
			return;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i) && i != ',')
			{
				return;
			}
		}
		vector<string> qqList = split(strqqnum, ",");
		for (int i = 0; i < qqList.size(); i++)
		{
			InsertBlack(stoll(qqList[i]),"�������", false);
			AddMsgToQueue("����Υ������ѱ�������������", stoll(qqList[i]));
			AddMsgToQueue("�ѽ�" + qqList[i] + "������������", eve.fromQQ);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 8) == "bangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 8;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strGroupnum = "";
		while (strLowerMessage[intMsgCnt])
		{
			strGroupnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strGroupnum == "")
		{
			AddMsgToQueue("��������Ⱥ�ţ�", eve.fromQQ);
			return;
		}
		for (auto i : strGroupnum)
		{
			if (!isdigit(i) && i != ',')
			{
				return;
			}
		}

		vector<string> qqGroupList = split(strGroupnum, ",");
		for (int i = 0; i < qqGroupList.size(); i++)
		{
			InsertBlack(stoll(qqGroupList[i]),"�������", true);
			AddMsgToQueue("Ⱥ" + qqGroupList[i] + "�ѱ�������������", stoll(qqGroupList[i]));
			AddMsgToQueue("�ѽ���Ⱥ" + qqGroupList[i] + "������������", eve.fromQQ);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "unban" && strLowerMessage.substr(intMsgCnt, 10) != "unbangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 5;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "")
		{
			AddMsgToQueue("��������QQ�ţ�", eve.fromQQ);
			eve.message_block();
			return;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i))
			{
				eve.message_block();
				return;
			}
		}
		set<long long> BanedQQList;

		BanedQQList = QueryBlack(false);

		if (!BanedQQList.count(stoll(strqqnum)))
		{
			AddMsgToQueue(strqqnum + "�����ں������У�", eve.fromQQ);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqnum), false);

		AddMsgToQueue(" ���ѱ��Ƴ����������", stoll(strqqnum));
		AddMsgToQueue("�ѽ�" + strqqnum + "�Ƴ����������", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 10) == "unbangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 10;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqGroupNum = "";
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqGroupNum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqGroupNum == "")
		{
			AddMsgToQueue("��������Ⱥ�ţ�", eve.fromQQ);
			return;
		}
		for (auto i : strqqGroupNum)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		set<long long> BanedGroup;
		BanedGroup = QueryBlack(true);
		if (!BanedGroup.count(stoll(strqqGroupNum)))
		{
			AddMsgToQueue(strqqGroupNum + "������Ⱥ�������У�", eve.fromQQ);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqGroupNum), true);

		AddMsgToQueue("��Ⱥ�ѱ��Ƴ����������", stoll(strqqGroupNum), false);
		AddMsgToQueue("�ѽ�" + strqqGroupNum + "�Ƴ����������", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 7) == "banlist")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 7;
		string banList = "���QQ�б�Ϊ:";
		string banedQQList = "";
		set<long long> BanedQQList;
		set<long long> BanedGroupList;

		BanedQQList = QueryBlack(false);
		set<long long>::iterator iter = BanedQQList.begin();
		while (iter != BanedQQList.end())
		{
			banedQQList += "\n" + to_string(*iter);
			iter++;
		}
		AddMsgToQueue(banList + banedQQList, MASTER);

		string banGroupList = "���Ⱥ�б�Ϊ:";
		string banedGroupList = "";
		BanedGroupList = QueryBlack(true);
		set<long long>::iterator iter_G = BanedGroupList.begin();
		while (iter_G != BanedGroupList.end())
		{
			banedGroupList += "\n" + to_string(*iter_G);
			iter_G++;
		}
		AddMsgToQueue(banGroupList + banedGroupList, MASTER);

		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 6) == "switch")
	{
		showswitch();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 6) == "airing") {
	if (getLoginQQ() == 3619921214) {
		return;
	}
	map<long long, string> groupList = getGroupList();
	for (auto it = groupList.begin(); it != groupList.end(); ++it) {
		AddMsgToQueue("�౾30�춳���лָ���������QQ��Ϊ3619921214��������÷˹�������Ǽ����ṩ���񣬱��Ų����ṩ�������ڼ������һ���¹��ܣ�����鿴��÷˹��helpָ�зз��", it->first, false);
	}
	}
	return;
}
EVE_GroupMsg_EX(__eventGroupMsg)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);

	set<long long> BanedGroupList;

	BanedGroupList = QueryBlack(true);

	if (BanedGroupList.count(eve.fromGroup) && Switch["DontRunningInBanGroup"]) {
		eve.message_ignore();
		eve.message_block();
		return;
	}

	if (BanedQQList.count(eve.fromQQ) && Switch["DontRunningInBanGroupForUser"])
	{
		eve.message_ignore();
		eve.message_block();
		return;
	}

	if (BanedQQList.count(eve.fromQQ) && Switch["LeaveGroupByUser"])
	{
		string strMsg = "BanListRequest��Ⱥ " + getGroupList()[eve.fromGroup] + "(" + to_string(eve.fromGroup) +
			")�з��ֺ�������Ա"+ getStrangerInfo(eve.fromQQ).nick + "(" + to_string(eve.fromQQ) + "),���Զ��˳���";
		AddMsgToQueue(strMsg, MASTERGroup,false);
		AddMsgToQueue(strMsg, eve.fromGroup, false);
		setGroupLeave(eve.fromGroup);
		return;
	}

	if (BanedGroupList.count(eve.fromGroup) && Switch["LeaveBanGroup"])
	{
		string strMsg = "BanListRequest�ں�����Ⱥ " + getGroupList()[eve.fromGroup] + "(" + to_string(eve.fromGroup) +
			")��,���Զ��˳���";
		AddMsgToQueue(strMsg, MASTERGroup, false);
		AddMsgToQueue(strMsg, eve.fromGroup, false);
		setGroupLeave(eve.fromGroup);
		return;
	}

	if (eve.isSystem())
	{
		if (Switch["LeaveForbiddenGroup"]) {
			if (eve.message.find("������Ա����") != string::npos &&
				eve.message.find(to_string(getLoginQQ())) != string::npos)
			{
				string strMsg = "BanListRequest��Ⱥ " + getGroupList()[eve.fromGroup] + "(" + to_string(eve.fromGroup) +
					")�б�����,�ѽ�Ⱥ���ڲ��Զ��˳���";

				

				setGroupLeave(eve.fromGroup);
				AddMsgToQueue(strMsg, MASTERGroup, false);
			}
			if (Switch["BanForbiddenGroup"]) {
				InsertBlack(eve.fromGroup,"������", true);
			}
			return;
		}
	}
	if (eve.isSystem())
		return;
	init(eve.message);
	init2(eve.message);
	if (eve.message[0] != '.')
		return;
	int intMsgCnt = 1;
	while (isspace(eve.message[intMsgCnt]))
		intMsgCnt++;
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	BanedQQList = QueryBlack(false);
	if (BanedQQList.count(eve.fromQQ) && eve.fromQQ != MASTER)
	{
		AddMsgToQueue("����˽����Ϣ�ѱ����أ���Ϊ�����ں�������", MASTERGroup, false);
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "exit")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 4;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strgroup = "";
		if (strgroup == "")
		{
			AddMsgToQueue("��������Ҫ�˳���Ⱥ�ţ�", MASTERGroup, false);
			return;
		}
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strgroup += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strgroup)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		const long long llgroup = stoll(strgroup);
		if (setGroupLeave(llgroup) == 0)
			AddMsgToQueue("�Ѵ�" + strgroup + "���˳�", MASTERGroup, false);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "ban" && strLowerMessage.substr(intMsgCnt, 7) != "banlist" && strLowerMessage.substr(intMsgCnt, 8) != "bangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 3;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (strLowerMessage[intMsgCnt])
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "")
		{
			AddMsgToQueue("��������QQ�ţ�", MASTERGroup, false);
			return;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i) && i != ',')
			{
				return;
			}
		}
		vector<string> qqList = split(strqqnum, ",");
		for (int i = 0; i < qqList.size(); i++)
		{
			InsertBlack(stoll(qqList[i]),"�������", false);
			AddMsgToQueue("����Υ������ѱ�������������", stoll(qqList[i]));
			AddMsgToQueue("�ѽ�" + qqList[i] + "������������", MASTERGroup, false);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 8) == "bangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 8;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strGroupnum = "";
		while (strLowerMessage[intMsgCnt])
		{
			strGroupnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strGroupnum == "")
		{
			AddMsgToQueue("��������Ⱥ�ţ�", MASTERGroup, false);
			return;
		}
		for (auto i : strGroupnum)
		{
			if (!isdigit(i) && i != ',')
			{
				return;
			}
		}

		vector<string> qqGroupList = split(strGroupnum, ",");
		for (int i = 0; i < qqGroupList.size(); i++)
		{
			InsertBlack(stoll(qqGroupList[i]),"���������", true);
			AddMsgToQueue("Ⱥ" + qqGroupList[i] + "�ѱ�������������", stoll(qqGroupList[i]));
			AddMsgToQueue("�ѽ���Ⱥ" + qqGroupList[i] + "������������", MASTERGroup, false);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "unban" && strLowerMessage.substr(intMsgCnt, 10) != "unbangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 5;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "")
		{
			AddMsgToQueue("��������QQ�ţ�", MASTERGroup, false);
			eve.message_block();
			return;
		}
		for (auto i : strqqnum)
		{
			if (!isdigit(i))
			{
				eve.message_block();
				return;
			}
		}
		set<long long> BanedQQList;

		BanedQQList = QueryBlack(false);

		if (!BanedQQList.count(stoll(strqqnum)))
		{
			AddMsgToQueue(strqqnum + "�����ں������У�", MASTERGroup, false);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqnum), false);

		AddMsgToQueue(" ���ѱ��Ƴ����������", stoll(strqqnum));
		AddMsgToQueue("�ѽ�" + strqqnum + "�Ƴ����������", MASTERGroup, false);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 10) == "unbangroup")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 10;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqGroupNum = "";
		while (isdigit(strLowerMessage[intMsgCnt]))
		{
			strqqGroupNum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqGroupNum == "")
		{
			AddMsgToQueue("��������Ⱥ�ţ�", MASTERGroup, false);
			return;
		}
		for (auto i : strqqGroupNum)
		{
			if (!isdigit(i))
			{
				return;
			}
		}
		set<long long> BanedGroup;
		BanedGroup = QueryBlack(true);
		if (!BanedGroup.count(stoll(strqqGroupNum)))
		{
			AddMsgToQueue(strqqGroupNum + "������Ⱥ�������У�", MASTERGroup, false);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqGroupNum), true);

		AddMsgToQueue("��Ⱥ�ѱ��Ƴ����������", stoll(strqqGroupNum), false);
		AddMsgToQueue("�ѽ�" + strqqGroupNum + "�Ƴ����������", MASTERGroup, false);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 7) == "banlist")
	{
		if (eve.fromQQ != MASTER)
			return;
		intMsgCnt += 7;
		string banList = "���QQ�б�Ϊ:";
		string banedQQList = "";
		set<long long> BanedQQList;
		set<long long> BanedGroupList;

		BanedQQList = QueryBlack(false);
		set<long long>::iterator iter = BanedQQList.begin();
		while (iter != BanedQQList.end())
		{
			banedQQList += "\n" + to_string(*iter);
			iter++;
		}
		AddMsgToQueue(banList + banedQQList, MASTERGroup, false);

		string banGroupList = "���Ⱥ�б�Ϊ:";
		string banedGroupList = "";
		BanedGroupList = QueryBlack(true);
		set<long long>::iterator iter_G = BanedGroupList.begin();
		while (iter_G != BanedGroupList.end())
		{
			banedGroupList += "\n" + to_string(*iter_G);
			iter_G++;
		}
		AddMsgToQueue(banGroupList + banedGroupList, MASTERGroup, false);

		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 6) == "switch")
	{
		showswitch();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 6) == "airing") {
		if (getLoginQQ() == 3619921214) {
			return;
		}
		map<long long,string> groupList=getGroupList();
		for (auto it = groupList.begin(); it != groupList.end(); ++it) {
			AddMsgToQueue("�౾30�춳���лָ���������QQ��Ϊ3619921214��������÷˹�������Ǽ����ṩ���񣬱��Ų����ṩ�������ڼ������һ���¹��ܣ�����鿴��÷˹��helpָ�зз��", it->first, false);
		}
	}
}

EVE_DiscussMsg_EX(__eventDiscussMsg)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);

	if (eve.isSystem())
		return;
	else if (BanedQQList.count(eve.fromQQ) && eve.fromQQ != MASTER)
	{
		eve.message_block();
		return;
	}
	else
		return;
}
EVE_System_GroupMemberDecrease(__eventSystem_GroupMemberDecrease)
{
	if (beingOperateQQ == getLoginQQ())
	{

		if (Switch["BanDetachGroup"]) {
			InsertBlack(fromGroup,"��"+getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")�߳�", true);
			string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
			AddMsgToQueue(strAt + "BanListRequest�ѽ�Ⱥ" + getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")�������������Ϊ���߳�", MASTERGroup, false);
		}

		if (Switch["BanDetachOperator"]){
			InsertBlack(fromQQ,"��Ⱥ"+ getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")���߳�", false);
			AddMsgToQueue("����Υ������ѱ�������������", fromQQ);
			string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
			AddMsgToQueue(strAt + "�ѽ�" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")������������" + "ԭ�򣺱��߳�Ⱥ" + getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")", MASTERGroup, false);
		}
	}
	return 1;
}
EVE_System_GroupMemberIncrease(__eventSystem_GroupMemberIncrease)
{
	if (beingOperateQQ == getLoginQQ())
	{
		if (getGroupMemberList(fromGroup).size() < 20)
		{
			string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
			AddMsgToQueue(strAt + "BanListRequest�յ�" + getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")��Ⱥ���룬��ȺС��20��QQ�����֪ͨ(Ⱥ����Ϊ:"+getGroupMemberList(fromGroup).size()+")�����Զ�ͬ��", MASTERGroup, false);
		}
		if (__eventRequest_AddGroup==null){
			string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
			AddMsgToQueue(strAt + "�յ�" + getGroupList()[fromGroup] + "(" + to_string(fromGroup) + ")�����������룬δ�ӵ�QQ��Ⱥ������ͣ����Զ�ͬ��", MASTERGroup, false);
		}
		AddMsgToQueue(Messages["GroupMsg"], fromGroup, false);
	}
	return 1;
}
EVE_Request_AddFriend(__eventRequest_AddFriend)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);
	if (BanedQQList.count(fromQQ))
	{
		AddMsgToQueue(
			"���ĺ����������޷����ܡ���Ϊ���ѱ����ڣ�����ԭ���Ǳ����߳���Ⱥ��",
			fromQQ);
		string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
		AddMsgToQueue(strAt + "�յ���������:" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")�ĺ����������Զ��ܾ�",
					  MASTERGroup, false);
		setFriendAddRequest(responseFlag, 2, "");
		return 1;
	}
	else if (Switch["agreeUser"]) {
		string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
		AddMsgToQueue(strAt + "�յ�" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")�ĺ����������Զ�ͬ��", MASTERGroup, false);
		setFriendAddRequest(responseFlag, 1, "");
		AddMsgToQueue(Messages["QQMsg"], fromQQ);
		return 1;
	}
}
EVE_Request_AddGroup(__eventRequest_AddGroup)
{
	if (subType == 2)
	{
		string strMsg = "Ⱥ����������ԣ�" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + "),Ⱥ��" + getGroupList()[fromGroup] + "(" +
						to_string(fromGroup) + ")��";
		set<long long> BanedGroup;

		BanedGroup = QueryBlack(true);
		set<long long> BanedQQList;
		BanedQQList = QueryBlack(false);
		if (BanedGroup.count(fromGroup) && Switch["refuseGroup"])
		{
			strMsg += "\n�Ѿܾ���Ⱥ�ں������У�";
			AddMsgToQueue(Messages["BanGroupMsg"], fromGroup);
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else if (BanedQQList.count(fromQQ) && Switch["refuseBanUser"])
		{
			strMsg += "\n�Ѿܾ����û��ں������У�";
			AddMsgToQueue(Messages["BanQQMsg"], fromQQ);
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else if (Switch["agreeGroup"])
		{
			strMsg += "��ͬ��";
			setGroupAddRequest(responseFlag, 2, 1, "");
		}
		string strAt = "[CQ:at,qq=" + to_string(MASTER) + "]";
		AddMsgToQueue(strAt + strMsg, MASTERGroup, false);
		return 1;
	}
	return 1;
}
EVE_Disable(__eventDisable)
{
	Enabled = false;
	return 0;
}
EVE_Exit(__eventExit)
{
	if (!Enabled)
		return 0;

	return 0;
}

EVE_Menu(__eventTestSwitch)
{
	ShowMainWindow(getAuthCode());
	return 0;
}

MUST_AppInfo_RETURN(CQAPPID);