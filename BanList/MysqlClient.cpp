#include <stdio.h>
#include <WinSock.h> //һ��Ҫ�������������winsock2.h
#include "mysql.h"   //����mysqlͷ�ļ�(һ�ַ�ʽ����vcĿ¼�������ã�һ�����ļ��п�������Ŀ¼��Ȼ����������)
#include <Windows.h>
#include <string>
#include "MysqlClient.h"
#include <strstream>
#include <sstream>
#include <set>
#include <map>
#include "CQLogger.h"
#include "DiceMsgSend.h"

using namespace CQ;
using namespace std;
//�������������Ҳ�����ڹ���--������������
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "libmysql.lib")
MYSQL_FIELD *fd;	//�ֶ�������
char field[32][32]; //���ֶ�����ά����
MYSQL_RES *res;		//����ṹ�������е�һ����ѯ�����
MYSQL_ROW column;   //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
char query[800];	//��ѯ���

//int main(int argc, char **argv)
//{
//	ConnectDatabase();
//	QueryDatabase1();
//	InsertData();
//	QueryDatabase2();
//	ModifyData();
//	QueryDatabase2();
//	DeleteData();
//	QueryDatabase2();
//	FreeConnect();
//	system("pause");
//	return 0;
//}

std::string btoi(bool value) {
	if (value == false) {
		return "false";
	}
	else {
		return "true";
	}
}

bool itob(int value)
{
	if (value == 0) {
		return false;
	}
	else {
		return true;
	}
}
string lltoString(long long t)
{
	std::string result;
	std::strstream ss;
	ss << t;
	ss >> result;
	return result;
}

long long my_atoll(char *instr)
{
	long long retval;
	int i;

	retval = 0;
	for (; *instr; instr++)
	{
		retval = 10 * retval + (*instr - '0');
	}
	return retval;
}

/***************************���ݿ����***********************************/
//��ʵ���е����ݿ����������д��sql��䣬Ȼ����mysql_query(&mysql,query)����ɣ������������ݿ�����ɾ�Ĳ�
//��ѯ����
set<long long> QueryBlack(bool group)
{
	set<long long> l1;
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		char *str = new char[20];
		if (group)
		{
			sprintf(query, "select GroupId from blackList"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		}
		else
		{
			sprintf(query, "select QQId from blackQQ"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		}
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);
		res = mysql_store_result(con);
		//��ȡ�ֶε���Ϣ
		char *str_field[32]; //����һ���ַ�������洢�ֶ���Ϣ
		str_field[1] = mysql_fetch_field(res)->name;
		while (column = mysql_fetch_row(res)) //����֪�ֶ���������£���ȡ����ӡ��һ��
		{
			l1.insert(my_atoll(column[0])); //column��������
		}
		mysql_free_result(res);
		mysql_close(con);
		return l1;
	}
}

//��������
bool InsertBlack(long long blackId,string reason bool group)
{
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		if (group)
		{
			sprintf(query, "insert into blackList(groupId,reason) values ('%s','%s');", lltoString(blackId).c_str(),reason.c_str()); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
		}
		else
		{
			sprintf(query, "insert into blackQQ(QQId,reason) values ('%s','%s');", lltoString(blackId).c_str(),reason.c_str()); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
		}
		if (mysql_query(con, query)) //ִ��SQL���
		{
			mysql_close(con);
			return false;
		}
		else
		{
			mysql_close(con);
			return true;
		}
	}
}

//ɾ������
bool DeleteBlack(long long blackId, bool group)
{
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		if (group)
		{
			sprintf(query, "delete from blackList where groupId='%s';", lltoString(blackId).c_str());
		}
		else
		{
			sprintf(query, "delete from blackQQ where QQId='%s';", lltoString(blackId).c_str());
		}
		if (mysql_query(con, query)) //ִ��SQL���
		{
			mysql_close(con);
			return false;
		}
		else
		{
			mysql_close(con);
			return true;
		}
	}
}

//�޸�����
bool ModifyData()
{
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		sprintf(query, "update user set email='lilei325@163.com' where name='Lilei'");
		if (mysql_query(con, query)) //ִ��SQL���
		{
			mysql_close(con);
			return false;
		}
		else
		{
			mysql_close(con);
			return true;
		}
	}
}

map<string, string> QueryMsg(long long loginQQ)
{
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		map<string, string> Messages;
		char *str = new char[20];
		sprintf(query, "select QQ,MASTER,MASTERGroup,QQMsg,GroupMsg,BanQQMsg,BanGroupMsg from Msg where QQ='%s';", to_string(loginQQ).c_str()); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		mysql_query(con, "set names gbk");																										//���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		res = mysql_store_result(con);
		//��ȡ�ֶε���Ϣ
		char *str_field[32]; //����һ���ַ�������洢�ֶ���Ϣ
		str_field[6] = mysql_fetch_field(res)->name;

		while (column = mysql_fetch_row(res)) //����֪�ֶ���������£���ȡ����ӡ��һ��
		{
			Messages["QQ"] = column[0];
			Messages["MASTER"] = column[1];
			Messages["MASTERGroup"] = column[2];
			Messages["QQMsg"] = column[3];
			Messages["GroupMsg"] = column[4];
			Messages["BanQQMsg"] = column[5];
			Messages["BanGroupMsg"] = column[6];
		}

		mysql_free_result(res);
		mysql_close(con);
		return Messages;
	}
}

map<string, bool> QuerySwitch(long long loginQQ)
{
	map<string, bool> Switch_tmp_query;
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
	
		char *str = new char[20];
		sprintf_s(query, "select LeaveBanGroup,refuseGroup,BanDetachOperator,deleteDetachOperator,agreeGroup,LeaveForbiddenGroup,BanForbiddenGroup,DontRunningInBanGroup,BanDetachGroup,LeaveGroupByUser,DontRunningInBanGroupForUser,refuseBanUser,agreeUser from Switch where QQ='%s';", to_string(loginQQ).c_str()); 
		mysql_query(con, "set names gbk");																										//���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		res = mysql_store_result(con);

		column = mysql_fetch_row(res);

			Switch_tmp_query["LeaveBanGroup"] = atoi(column[0]);
			Switch_tmp_query["refuseGroup"] = atoi(column[1]);
			Switch_tmp_query["BanDetachOperator"] = atoi(column[2]);
			Switch_tmp_query["deleteDetachOperator"] = atoi(column[3]);
			Switch_tmp_query["agreeGroup"] = atoi(column[4]);
			Switch_tmp_query["LeaveForbiddenGroup"] = atoi(column[5]);
			Switch_tmp_query["BanForbiddenGroup"] = atoi(column[6]);
			Switch_tmp_query["DontRunningInBanGroup"] = atoi(column[7]);
			Switch_tmp_query["BanDetachGroup"] = atoi(column[8]);

			Switch_tmp_query["LeaveGroupByUser"] = atoi(column[9]);
			Switch_tmp_query["DontRunningInBanGroupForUser"] = atoi(column[10]);
			Switch_tmp_query["refuseBanUser"] = atoi(column[11]);
			Switch_tmp_query["agreeUser"] = atoi(column[12]);


		mysql_free_result(res);
		mysql_close(con);

	}
	return Switch_tmp_query;
}

//��������
bool InsertMsg(map<string, string> Messages, long long loginQQ)
{
	MYSQL *con = mysql_init((MYSQL *)0);
	MYSQL_RES *rer;
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������

		sprintf_s(query, "select QQ from Msg where QQ='%s';", to_string(loginQQ).c_str()); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		rer = mysql_store_result(con);

		if (mysql_num_rows(rer) == 0)
		{

			sprintf_s(query, "insert into Msg(QQ,MASTER,MASTERGroup,QQMsg,GroupMsg,BanQQMsg,BanGroupMsg) values ('%s','%s','%s','%s','%s','%s','%s');",
				to_string(loginQQ).c_str(),
				Messages["MASTER"].c_str(),
				Messages["MASTERGroup"].c_str(),
				Messages["QQMsg"].c_str(),
				Messages["GroupMsg"].c_str(),
				Messages["BanQQMsg"].c_str(),
				Messages["BanGroupMsg"].c_str()); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��

			mysql_query(con, query);
		}
		else
		{
			sprintf_s(query, "update Msg set MASTER='%s' , MASTERGroup='%s' , QQMsg='%s' , GroupMsg='%s' , BanQQMsg='%s' , BanGroupMsg='%s' where QQ='%s';",
				Messages["MASTER"].c_str(),
				Messages["MASTERGroup"].c_str(),
				Messages["QQMsg"].c_str(),
				Messages["GroupMsg"].c_str(),
				Messages["BanQQMsg"].c_str(),
				Messages["BanGroupMsg"].c_str(),
				to_string(loginQQ).c_str()); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��

			mysql_query(con, query);
		}
	}
	mysql_free_result(rer);
	mysql_close(con);
	return true;
}

//��������
bool InsertSwitch(map<string, bool> Switch_tmp, long long loginQQ)
{
	int resultNum = 0;
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		map<string, bool> Switch;
		con->reconnect = 1;
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������

		sprintf_s(query, "select QQ from Switch where QQ='%s';", to_string(loginQQ).c_str()); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		mysql_query(con, "set names gbk");													//���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		res = mysql_store_result(con);

		resultNum = mysql_num_rows(res);
		if (resultNum == 0)
		{
			sprintf_s(query, "insert into Switch(QQ,LeaveBanGroup,refuseGroup,BanDetachOperator,deleteDetachOperator,agreeGroup,LeaveForbiddenGroup,BanForbiddenGroup,DontRunningInBanGroup,BanDetachGroup,LeaveGroupByUser,DontRunningInBanGroupForUser,refuseBanUser,agreeUser) values ('%s','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",
					to_string(loginQQ).c_str(),
					Switch_tmp["LeaveBanGroup"],
					Switch_tmp["refuseGroup"],
					Switch_tmp["BanDetachOperator"],
					Switch_tmp["deleteDetachOperator"],
					Switch_tmp["agreeGroup"],
					Switch_tmp["LeaveForbiddenGroup"],
					Switch_tmp["BanForbiddenGroup"],
					Switch_tmp["DontRunningInBanGroup"],
					Switch_tmp["BanDetachGroup"],
					Switch_tmp["LeaveGroupByUser"],
					Switch_tmp["DontRunningInBanGroupForUser"],
					Switch_tmp["refuseBanUser"],
					Switch_tmp["agreeUser"]);
				mysql_query(con, query);

		}
		else {

				sprintf_s(query, "update Switch set LeaveBanGroup= '%d',refuseGroup= '%d',BanDetachOperator= '%d',deleteDetachOperator= '%d',agreeGroup= '%d',LeaveForbiddenGroup= '%d',BanForbiddenGroup= '%d',DontRunningInBanGroup= '%d',BanDetachGroup= '%d',LeaveGroupByUser= '%d',DontRunningInBanGroupForUser= '%d',refuseBanUser= '%d',agreeUser= '%d' where QQ='%s';",
					Switch_tmp["LeaveBanGroup"],
					Switch_tmp["refuseGroup"],
					Switch_tmp["BanDetachOperator"],
					Switch_tmp["deleteDetachOperator"],
					Switch_tmp["agreeGroup"],
					Switch_tmp["LeaveForbiddenGroup"],
					Switch_tmp["BanForbiddenGroup"],
					Switch_tmp["DontRunningInBanGroup"],
					Switch_tmp["BanDetachGroup"],
					Switch_tmp["LeaveGroupByUser"],
					Switch_tmp["DontRunningInBanGroupForUser"],
					Switch_tmp["refuseBanUser"],
					Switch_tmp["agreeUser"],
					to_string(loginQQ).c_str());


			mysql_query(con, query);
		}
	}
	mysql_free_result(res);
	mysql_close(con);
	return true;
}



//��������
bool initMsg(long long loginQQ)
{
	int resultNum = 0;
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������

		sprintf(query, "select QQ from Msg where QQ='%s';", to_string(loginQQ).c_str()); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		mysql_query(con, "set names gbk");												 //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		res = mysql_store_result(con);

		resultNum = mysql_num_rows(res);
		if (resultNum == 0)
		{
			map<string, string> Messages;
			Messages["MASTER"] = "0";
			Messages["MASTERGroup"] = "0";
			Messages["QQMsg"] = "���ã������������Ϊ����";
			Messages["GroupMsg"] = "��λ�ã������Ѽ��뱾Ⱥ";
			Messages["BanQQMsg"] = "�ܱ�Ǹ�����ѱ����ڣ��ܾ�����";
			Messages["BanGroupMsg"] = "�ܱ�Ǹ����Ⱥ�ѱ����ڣ��ܾ�����";
			sprintf(query, "insert into Msg(QQ,MASTER,MASTERGroup,QQMsg,GroupMsg,BanQQMsg,BanGroupMsg) values ('%s','%s','%s','%s','%s','%s','%s');",
				to_string(loginQQ).c_str(),
				Messages["MASTER"].c_str(),
				Messages["MASTERGroup"].c_str(),
				Messages["QQMsg"].c_str(),
				Messages["GroupMsg"].c_str(),
				Messages["BanQQMsg"].c_str(),
				Messages["BanGroupMsg"].c_str()); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
			if (mysql_query(con, query))			  //ִ��SQL���
			{
				mysql_close(con);
				return false;
			}
			else
			{
				mysql_free_result(res);
				mysql_close(con);
				return true;
			}
		}
		else
		{
			mysql_free_result(res);
			mysql_close(con);
			return true;
		}
	}
}

//��������
bool initSwitch(long long loginQQ)
{
	int resultNum = 0;
	MYSQL *con = mysql_init((MYSQL *)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))
	{
		con->reconnect = 1;
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������

		sprintf(query, "select QQ from Switch where QQ='%s';", to_string(loginQQ).c_str()); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		mysql_query(con, "set names gbk");													//���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);

		res = mysql_store_result(con);

		resultNum = mysql_num_rows(res);
		if (resultNum == 0)
		{
			map<string, bool> Switch_tmp;
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
			sprintf(query, "insert into Switch(QQ,LeaveBanGroup,refuseGroup,BanDetachOperator,deleteDetachOperator,agreeGroup,LeaveForbiddenGroup,BanForbiddenGroup,DontRunningInBanGroup,BanDetachGroup,LeaveGroupByUser,DontRunningInBanGroupForUser,refuseBanUser,agreeUser) values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');",
				to_string(loginQQ).c_str(),
				Switch_tmp["LeaveBanGroup"],
				Switch_tmp["refuseGroup"],
				Switch_tmp["BanDetachOperator"],
				Switch_tmp["deleteDetachOperator"],
				Switch_tmp["agreeGroup"],
				Switch_tmp["LeaveForbiddenGroup"],
				Switch_tmp["BanForbiddenGroup"],
				Switch_tmp["DontRunningInBanGroup"],
				Switch_tmp["BanDetachGroup"],
				Switch_tmp["LeaveGroupByUser"],
				Switch_tmp["DontRunningInBanGroupForUser"],
				Switch_tmp["refuseBanUser"],
				Switch_tmp["agreeUser"]); //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
			if (mysql_query(con, query))			  //ִ��SQL���
			{
				mysql_close(con);
				return false;
			}
			else
			{
				mysql_free_result(res);
				mysql_close(con);
				return true;
			}
		}
		else
		{
			mysql_free_result(res);
			mysql_close(con);
			return true;
		}
	}
}