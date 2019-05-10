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

#include "APPINFO.h"
#include "CQEVE_ALL.h"
#include "CQTools.h"
#include "GlobalVar.h"
#include "DiceMsgSend.h"

using namespace std;
using namespace CQ;
const int MASTER = 450609203;

inline void init(string &msg) {
    msg_decode(msg);
}

inline void init2(string &msg) {
    for (int i = 0; i != msg.length(); i++) {
        if (msg[i] < 0) {
            if ((msg[i] & 0xff) == 0xa1 && (msg[i + 1] & 0xff) == 0xa1) {
                msg[i] = 0x20;
                msg.erase(msg.begin() + i + 1);
            } else if ((msg[i] & 0xff) == 0xa3 && (msg[i + 1] & 0xff) >= 0xa1 && (msg[i + 1] & 0xff) <= 0xfe) {
                msg[i] = msg[i + 1] - 0x80;
                msg.erase(msg.begin() + i + 1);
            } else {
                i++;
            }
        }
    }

    while (isspace(msg[0]))
        msg.erase(msg.begin());
    while (!msg.empty() && isspace(msg[msg.length() - 1]))
        msg.erase(msg.end() - 1);
    if (msg.substr(0, 2) == "��") {
        msg.erase(msg.begin());
        msg[0] = '.';
    }
    if (msg[0] == '!')
        msg[0] = '.';
}

std::string strip(std::string origin) {
    bool flag = true;
    while (flag) {
        flag = false;
        if (origin[0] == '!' || origin[0] == '.') {
            origin.erase(origin.begin());
            flag = true;
        } else if (origin.substr(0, 2) == "��" || origin.substr(0, 2) == "��") {
            origin.erase(origin.begin());
            origin.erase(origin.begin());
            flag = true;
        }
    }
    return origin;
}

set<long long> BanedQQ;
set<long long> BanedGroup;

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
string strFileLoc;
EVE_Enable(__eventEnable)
        {
                //Wait until the thread terminates
                while (msgSendThreadRunning)
                Sleep(10);

                thread msgSendThread(SendMsg);
                msgSendThread.detach();
                strFileLoc = getAppDirectory();
                /*
                * ���ƴ洢-�������ȡ
                */
                ifstream ifstreamBanedQQ(strFileLoc + "BanedQQ.RDconf");
                if (ifstreamBanedQQ)
                {
                    long long QQNum;
                    while (ifstreamBanedQQ >> QQNum) {
                        BanedQQ.insert(QQNum);
                    }
                }
                ifstreamBanedQQ.close();

                ifstream ifstreamBanedGroup(strFileLoc + "BanedGroup.RDconf");
                if (ifstreamBanedGroup)
                {
                    long long GroupNum;
                    while (ifstreamBanedGroup >> GroupNum) {
                        BanedGroup.insert(GroupNum);
                    }
                }
                ifstreamBanedGroup.close();

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
                if (BanedQQ.count(eve.fromQQ) && eve.fromQQ != MASTER)
                {
					AddMsgToQueue("����˽����Ϣ�ѱ����أ���Ϊ�����ں�������", eve.fromQQ);
                    eve.message_block();
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
                        AddMsgToQueue("��������Ҫ�˳���Ⱥ�ţ�", eve.fromQQ);
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
                        AddMsgToQueue("�Ѵ�" + strgroup + "���˳�", eve.fromQQ);
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
                    while (isdigit(strLowerMessage[intMsgCnt])) {
                        strqqnum += strLowerMessage[intMsgCnt];
                        intMsgCnt++;
                    }
                    if (strqqnum == "") {
                        AddMsgToQueue("��������QQ�ţ�", eve.fromQQ);
                        return;
                    }
                    for (auto i : strqqnum) {
                        if (!isdigit(i)) {
                            return;
                        }
                    }
                    const long long llqqnum = stoll(strqqnum);
                    BanedQQ.insert(llqqnum);
                    AddMsgToQueue("����Υ������ѱ�������������", llqqnum);
                    AddMsgToQueue("�ѽ�" + strqqnum + "������������", eve.fromQQ);
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
                    while (isdigit(strLowerMessage[intMsgCnt])) {
                        strGroupnum += strLowerMessage[intMsgCnt];
                        intMsgCnt++;
                    }
                    if (strGroupnum == "") {
                        AddMsgToQueue("��������Ⱥ�ţ�", eve.fromQQ);
                        return;
                    }
                    for (auto i : strGroupnum) {
                        if (!isdigit(i)) {
                            return;
                        }
                    }
                    const long long llqqnum = stoll(strGroupnum);
                    BanedGroup.insert(llqqnum);
                    AddMsgToQueue("Ⱥ" + strGroupnum + "�ѱ�������������", llqqnum);
                    AddMsgToQueue("�ѽ���Ⱥ" + strGroupnum + "������������", eve.fromQQ);
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
                        AddMsgToQueue("��������QQ�ţ�", eve.fromQQ);
                        eve.message_block();
                        return;
                    }
                    for (auto i : strqqnum) {
                        if (!isdigit(i)) {
                            eve.message_block();
                            return;
                        }
                    }
                    if (!BanedQQ.count(stoll(strqqnum))) {
                        AddMsgToQueue(strqqnum + "�����ں������У�", eve.fromQQ);
                        eve.message_block();
                        return;
                    }
                    const long long llqqnum = stoll(strqqnum);

                    BanedQQ.erase(llqqnum);
                    AddMsgToQueue(" ���ѱ��Ƴ����������", llqqnum);
                    AddMsgToQueue("�ѽ�" + strqqnum + "�Ƴ����������", eve.fromQQ);
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
                        AddMsgToQueue("��������Ⱥ�ţ�", eve.fromQQ);
                        return;
                    }
                    for (auto i : strqqGroupNum) {
                        if (!isdigit(i)) {
                            return;
                        }
                    }
                    if (!BanedGroup.count(stoll(strqqGroupNum))) {
                        AddMsgToQueue(strqqGroupNum + "������Ⱥ�������У�", eve.fromQQ);
                    }
                    const long long llqqnum = stoll(strqqGroupNum);

                    BanedGroup.erase(llqqnum);
                    AddMsgToQueue("��Ⱥ�ѱ��Ƴ����������", llqqnum, false);
                    AddMsgToQueue("�ѽ�" + strqqGroupNum + "�Ƴ����������", eve.fromQQ);
                    eve.message_block();
                    return;
                }
                else if (strLowerMessage.substr(intMsgCnt, 7) == "banlist")
                {
                    if (eve.fromQQ != MASTER) return;
                    intMsgCnt += 7;
                    string banList = "���QQ�б�Ϊ:";
                    set<long long>::iterator iter = BanedQQ.begin();
                    while (iter != BanedQQ.end()) {
                        banList += "\n" + to_string(*iter);
                        iter++;
                    }
                    AddMsgToQueue(banList, MASTER);
                    string banGroupList = "���Ⱥ�б�Ϊ:";
                    set<long long>::iterator iterGroup = BanedGroup.begin();
                    while (iterGroup != BanedGroup.end()) {
                        banGroupList += "\n" + to_string(*iterGroup);
                        iterGroup++;
                    }
                    AddMsgToQueue(banGroupList, MASTER);
                    eve.message_block();
                    return;
                }
				else if (strLowerMessage.substr(intMsgCnt, 5) == "mysql")
				{
				intMsgCnt += 5;
				string tmp;
				list<string> a = QueryBlack();
				//typedef std::list<string> list_t;
				//list_t::iterator iter;
				//for (iter = a.begin(); iter != a.end(); iter++)
				//{
				//	AddMsgToQueue(*iter, MASTER);
				//}
				
				eve.message_block();
				return;
				}
                return;
        }
EVE_GroupMsg_EX(__eventGroupMsg)
        {
                if (eve.isAnonymous()) {
                    return;
                }
                if (BanedQQ.count(eve.fromQQ)) {
                    eve.message_ignore();
                    eve.message_block();
                    return;
                }
                if (eve.isSystem()) {
                    if (eve.message.find("������Ա����") != string::npos &&
                        eve.message.find(to_string(getLoginQQ())) != string::npos) {
                        string strMsg = "��Ⱥ\"" + getGroupList()[eve.fromGroup] + "\"(" + to_string(eve.fromGroup) +
                                        ")�б�����,�ѽ�Ⱥ���ڲ��Զ��˳���";
                        AddMsgToQueue(strMsg, MASTER);
                        BanedGroup.insert(eve.fromGroup);
                        setGroupLeave(eve.fromGroup);
                    }
                    return;
                }
        }

EVE_DiscussMsg_EX(__eventDiscussMsg)
        {
                if (eve.isSystem())return;
                else if (BanedQQ.count(eve.fromQQ) && eve.fromQQ != MASTER)
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
                    BanedQQ.insert(fromQQ);
                    BanedGroup.insert(fromGroup);
                    AddMsgToQueue("����Υ������ѱ�������������", fromQQ);
                    AddMsgToQueue("�ѽ�" + to_string(fromQQ) + "������������" + "ԭ�򣺱��߳�Ⱥ" + to_string(fromGroup), MASTER);
                    return 1;
                }
                return 0;
        }
EVE_System_GroupMemberIncrease(__eventSystem_GroupMemberIncrease)
        {
                if (beingOperateQQ == getLoginQQ())
                {
                    AddMsgToQueue("��Ⱥ��Ϣ", fromGroup, false);
                }
                return 1;
        }
EVE_Request_AddFriend(__eventRequest_AddFriend)
        {
        if (BanedQQ.count(fromQQ)) {
            AddMsgToQueue(
                    "���ĺ����������޷����ܡ���Ϊ���ѱ����ڣ�����ԭ���Ǳ����߳���Ⱥ��",
                    fromQQ);
			setFriendAddRequest(responseFlag, 2, "");
            return 1;
        }
        setFriendAddRequest(responseFlag, 1, "");
        return 0;
        }
EVE_Request_AddGroup(__eventRequest_AddGroup)
        {
        setGroupAddRequest(responseFlag, 2, 2, "");
        if (subType == 2) {
            string strMsg = "Ⱥ����������ԣ�" + getStrangerInfo(fromQQ).nick + "(" + to_string(fromQQ) + "),Ⱥ��(" +
                            to_string(fromGroup) + ")��";
            if (BanedGroup.count(fromGroup)) {
                strMsg += "\n�Ѿܾ���Ⱥ�ں������У�";
                setGroupAddRequest(responseFlag, 2, 2, "");
            } else if (BanedQQ.count(fromQQ)) {
                strMsg += "\n�Ѿܾ����û��ں������У�";
                setGroupAddRequest(responseFlag, 2, 2, "");
            } else {
                strMsg += "��ͬ��";
                setGroupAddRequest(responseFlag, 2, 1, "");
            }
            AddMsgToQueue(strMsg, MASTER);
            return 1;
        }
        return 1;
        }
EVE_Disable(__eventDisable)
        {
                Enabled = false;
        ofstream ofstreamBanedQQ(strFileLoc + "BanedQQ.RDconf", ios::out | ios::trunc);
        for (auto it = BanedQQ.begin(); it != BanedQQ.end(); ++it)
        {
            ofstreamBanedQQ << *it << std::endl;
        }
        ofstreamBanedQQ.close();

        ofstream ofstreamBanedGroup(strFileLoc + "BanedGroup.RDconf", ios::out | ios::trunc);
        for (auto it = BanedGroup.begin(); it != BanedGroup.end(); ++it)
        {
            ofstreamBanedGroup << *it << std::endl;
        }
        ofstreamBanedGroup.close();

        return 0;
        }
EVE_Exit(__eventExit)
        {
                if (!Enabled)
                return 0;
                ofstream ofstreamBanedQQ(strFileLoc + "BanedQQ.RDconf", ios::out | ios::trunc);
                for (auto it = BanedQQ.begin(); it != BanedQQ.end(); ++it)
                {
                    ofstreamBanedQQ << *it << std::endl;
                }
                ofstreamBanedQQ.close();

                ofstream ofstreamBanedGroup(strFileLoc + "BanedGroup.RDconf", ios::out | ios::trunc);
                for (auto it = BanedGroup.begin(); it != BanedGroup.end(); ++it)
                {
                    ofstreamBanedGroup << *it << std::endl;
                }
                ofstreamBanedGroup.close();

                return 0;
        }
MUST_AppInfo_RETURN(CQAPPID);