/*
* Dice! QQ Dice Robot for TRPG
* Copyright (C) 2018 w4123溯洄
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

#include "APPINFO.h"
#include "CQEVE_ALL.h"
#include "CQTools.h"
#include "GlobalVar.h"
#include "DiceMsgSend.h"
#include "NameStorage.h"

using namespace std;
using namespace CQ;
const int MASTER = 450609203;
const int MasterGroup = 112380103;

unique_ptr<NameStorage> Name;

inline void init(string &msg) {
	msg_decode(msg);
}

inline void init2(string &msg) {
	for (int i = 0; i != msg.length(); i++) {
		if (msg[i] < 0) {
			if ((msg[i] & 0xff) == 0xa1 && (msg[i + 1] & 0xff) == 0xa1) {
				msg[i] = 0x20;
				msg.erase(msg.begin() + i + 1);
			}
			else if ((msg[i] & 0xff) == 0xa3 && (msg[i + 1] & 0xff) >= 0xa1 && (msg[i + 1] & 0xff) <= 0xfe) {
				msg[i] = msg[i + 1] - 0x80;
				msg.erase(msg.begin() + i + 1);
			}
			else {
				i++;
			}
		}
	}

	while (isspace(msg[0]))
		msg.erase(msg.begin());
	while (!msg.empty() && isspace(msg[msg.length() - 1]))
		msg.erase(msg.end() - 1);
	if (msg.substr(0, 2) == "。") {
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
	//先将要切割的字符串从string类型转换为char*类型
	char *strs = new char[str.length() + 1]; //不要忘了
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;	 //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}

	return res;
}


std::string strip(std::string origin) {
	bool flag = true;
	while (flag) {
		flag = false;
		if (origin[0] == '!' || origin[0] == '.') {
			origin.erase(origin.begin());
			flag = true;
		}
		else if (origin.substr(0, 2) == "！" || origin.substr(0, 2) == "。") {
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
		/*群*/
		if (GroupID < 1000000000)
		{
			return strip(Name->get(GroupID, QQ).empty()
				? (getGroupMemberInfo(GroupID, QQ).GroupNick.empty()
					? getStrangerInfo(QQ).nick
					: getGroupMemberInfo(GroupID, QQ).GroupNick)
				: Name->get(GroupID, QQ));
		}
		/*讨论组*/
		return strip(Name->get(GroupID, QQ).empty() ? getStrangerInfo(QQ).nick : Name->get(GroupID, QQ));
	}
	/*私聊*/
	return strip(getStrangerInfo(QQ).nick);
}

struct SourceType {
	SourceType(long long a, int b, long long c) : QQ(a), Type(b), GrouporDiscussID(c) {};
	long long QQ = 0;
	int Type = 0;
	long long GrouporDiscussID = 0;

	bool operator<(SourceType b) const {
		return this->QQ < b.QQ;
	}
};

using PropType = map<string, int>;
EVE_Enable(__eventEnable)
{
	//Wait until the thread terminates
	while (msgSendThreadRunning)
		Sleep(10);

	thread msgSendThread(SendMsg);
	msgSendThread.detach();
	return 0;
}

EVE_PrivateMsg_EX(__eventPrivateMsg)
{
	if (eve.isSystem())return;
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
		AddMsgToQueue("您的私聊信息已被拦截，因为您处于黑名单中", eve.fromQQ);
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "exit")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 4;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strgroup = "";
		if (strgroup == "") {
			AddMsgToQueue("请输入需要退出的群号！", eve.fromQQ);
			return;
		}
		while (isdigit(strLowerMessage[intMsgCnt])) {
			strgroup += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		for (auto i : strgroup) {
			if (!isdigit(i)) {
				return;
			}
		}
		const long long llgroup = stoll(strgroup);
		if (setGroupLeave(llgroup) == 0)
			AddMsgToQueue("已从" + strgroup + "中退出", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "ban" && strLowerMessage.substr(intMsgCnt, 7) != "banlist"  && strLowerMessage.substr(intMsgCnt, 8) != "bangroup")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 3;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (strLowerMessage[intMsgCnt]) {
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "") {
			AddMsgToQueue("请输入封禁QQ号！", eve.fromQQ);
			return;
		}
		for (auto i : strqqnum) {
			if (!isdigit(i) && i != ',') {
				return;
			}
		}
		vector<string> qqList = split(strqqnum, ",");
		for (int i = 0; i < qqList.size(); i++)
		{
			InsertBlack(stoll(qqList[i]), false);
			AddMsgToQueue("您因违规操作已被列入封禁名单！", stoll(qqList[i]));
			AddMsgToQueue("已将" + qqList[i] + "列入封禁名单！", eve.fromQQ);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 8) == "bangroup")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 8;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strGroupnum = "";
		while (strLowerMessage[intMsgCnt]) {
			strGroupnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strGroupnum == "") {
			AddMsgToQueue("请输入封禁群号！", eve.fromQQ);
			return;
		}
		for (auto i : strGroupnum) {
			if (!isdigit(i) && i != ',') {
				return;
			}
		}

		vector<string> qqGroupList = split(strGroupnum, ",");
		for (int i = 0; i < qqGroupList.size(); i++)
		{
			InsertBlack(stoll(qqGroupList[i]), true);
			AddMsgToQueue("群" + qqGroupList[i] + "已被列入封禁名单！", stoll(qqGroupList[i]));
			AddMsgToQueue("已将此群" + qqGroupList[i] + "列入封禁名单！", eve.fromQQ);
		}
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "unban" && strLowerMessage.substr(intMsgCnt, 10) != "unbangroup")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 5;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqnum = "";
		while (isdigit(strLowerMessage[intMsgCnt])) {
			strqqnum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqnum == "") {
			AddMsgToQueue("请输入解封QQ号！", eve.fromQQ);
			eve.message_block();
			return;
		}
		for (auto i : strqqnum) {
			if (!isdigit(i)) {
				eve.message_block();
				return;
			}
		}
		set<long long> BanedQQList;

		BanedQQList = QueryBlack(false);

		if (!BanedQQList.count(stoll(strqqnum))) {
			AddMsgToQueue(strqqnum + "并不在黑名单中！", eve.fromQQ);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqnum), false);

		AddMsgToQueue(" 您已被移出封禁名单！", stoll(strqqnum));
		AddMsgToQueue("已将" + strqqnum + "移出封禁名单！", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 10) == "unbangroup")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 10;
		while (isspace(strLowerMessage[intMsgCnt]))
			intMsgCnt++;
		string strqqGroupNum = "";
		while (isdigit(strLowerMessage[intMsgCnt])) {
			strqqGroupNum += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		if (strqqGroupNum == "") {
			AddMsgToQueue("请输入解封群号！", eve.fromQQ);
			return;
		}
		for (auto i : strqqGroupNum) {
			if (!isdigit(i)) {
				return;
			}
		}
		set<long long> BanedGroup;
		BanedGroup = QueryBlack(true);
		if (!BanedGroup.count(stoll(strqqGroupNum))) {
			AddMsgToQueue(strqqGroupNum + "并不在群黑名单中！", eve.fromQQ);
			eve.message_block();
			return;
		}

		DeleteBlack(stoll(strqqGroupNum), true);

		AddMsgToQueue("此群已被移出封禁名单！", stoll(strqqGroupNum), false);
		AddMsgToQueue("已将" + strqqGroupNum + "移出封禁名单！", eve.fromQQ);
		eve.message_block();
		return;
	}
	else if (strLowerMessage.substr(intMsgCnt, 7) == "banlist")
	{
		if (eve.fromQQ != MASTER) return;
		intMsgCnt += 7;
		string banList = "封禁QQ列表为:";
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

		string banGroupList = "封禁群列表为:";
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



	return;
}
EVE_GroupMsg_EX(__eventGroupMsg)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);

	if (BanedQQList.count(eve.fromQQ)) {
		eve.message_ignore();
		eve.message_block();
		return;
	}
	if (eve.isSystem()) {
		if (eve.message.find("被管理员禁言") != string::npos &&
			eve.message.find(to_string(getLoginQQ())) != string::npos) {
			string strMsg = "在群 " + getGroupList()[eve.fromGroup] + "(" + to_string(eve.fromGroup) +
				")中被禁言,已将群拉黑并自动退出。";

			InsertBlack(eve.fromGroup, true);

			setGroupLeave(eve.fromGroup);
			AddMsgToQueue(strMsg, MASTER);
			AddMsgToQueue(strMsg, MasterGroup, false);
		}
		return;
	}

}

EVE_DiscussMsg_EX(__eventDiscussMsg)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);

	if (eve.isSystem())return;
	else if (BanedQQList.count(eve.fromQQ) && eve.fromQQ != MASTER)
	{
		eve.message_block();
		return;
	}
	else return;
}
EVE_System_GroupMemberDecrease(__eventSystem_GroupMemberDecrease)
{
	if (beingOperateQQ == getLoginQQ())
	{

		InsertBlack(fromQQ, false);
		InsertBlack(fromGroup, true);

		AddMsgToQueue("您因违规操作已被列入封禁名单！", fromQQ);
		AddMsgToQueue("已将" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")列入封禁名单！" + "原因：被踢出群" + getGroupList()[fromGroup]+"("+to_string(fromGroup)+")", MASTER);
		AddMsgToQueue("已将" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")列入封禁名单！" + "原因：被踢出群" + getGroupList()[fromGroup]+"("+to_string(fromGroup)+")", MasterGroup, false);
		return 1;
	}
	return 0;
}
EVE_System_GroupMemberIncrease(__eventSystem_GroupMemberIncrease)
{
	if (beingOperateQQ == getLoginQQ())
	{
		if (getGroupList().size() < 20) {
			AddMsgToQueue("收到" + getGroupList()[fromGroup]+"("+to_string(fromGroup) + ")的群邀请，因群小于20人QQ无审核通知，已自动同意", MasterGroup, false);
		}
		AddMsgToQueue("各位好，这里是缇娜·里歇尔，原坂本酱\n本骰子持有十多种与跑团相关的独有增强功能，详情查看.help下半部分", fromGroup, false);
	}
	return 1;
}
EVE_Request_AddFriend(__eventRequest_AddFriend)
{
	set<long long> BanedQQList;

	BanedQQList = QueryBlack(false);
	if (BanedQQList.count(fromQQ)) {
		AddMsgToQueue(
			"您的好友邀请我无法接受。因为您已被拉黑，拉黑原因是被您踢出过群。",
			fromQQ);
		AddMsgToQueue(
			"收到黑名单内:" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")的好友请求，已自动拒绝",
			MASTER);
		AddMsgToQueue("收到黑名单内:" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")的好友请求，已自动拒绝",
			MasterGroup, false);
		setFriendAddRequest(responseFlag, 2, "您的好友邀请我无法接受。因为您已被拉黑，拉黑原因是被您踢出过群。");
		return 1;
	}
	AddMsgToQueue("收到" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + ")的好友请求，已自动同意", MasterGroup, false);
	setFriendAddRequest(responseFlag, 1, "各位好，这里是缇娜·里歇尔，原坂本酱\n本骰子持有十多种与跑团相关的独有增强功能，详情查看.help下半部分");
	return 1;
}
EVE_Request_AddGroup(__eventRequest_AddGroup)
{
	setGroupAddRequest(responseFlag, 2, 1, "");
	if (subType == 2) {
		string strMsg = "群添加请求，来自：" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + "),群：(" +
			to_string(fromGroup) + ")。";
		set<long long> BanedGroup;

		BanedGroup = QueryBlack(true);
		set<long long> BanedQQList;
		BanedQQList = QueryBlack(false);
		AddMsgToQueue(to_string(fromGroup), fromQQ);
		if (BanedGroup.count(fromGroup)) {
			strMsg += "\n已拒绝（群在黑名单中）";
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else if (BanedQQList.count(fromQQ)) {
			strMsg += "\n已拒绝（用户在黑名单中）";
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else {
			strMsg += "已同意";
			setGroupAddRequest(responseFlag, 2, 1, "");
		}
		AddMsgToQueue(strMsg, MasterGroup, false);
		return 1;
	}
	return 0;
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
MUST_AppInfo_RETURN(CQAPPID);