#include <stdio.h>
#include <WinSock.h>  //һ��Ҫ�������������winsock2.h
#include "mysql.h"    //����mysqlͷ�ļ�(һ�ַ�ʽ����vcĿ¼�������ã�һ�����ļ��п�������Ŀ¼��Ȼ����������)
#include <Windows.h>
#include <string>
#include "MysqlClient.h"
#include <strstream>
#include <sstream>
#include <set>

using namespace std;
//�������������Ҳ�����ڹ���--������������
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL_FIELD *fd;  //�ֶ�������
char field[32][32];  //���ֶ�����ά����
MYSQL_RES *res; //����ṹ�������е�һ����ѯ�����
MYSQL_ROW column; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
char query[150]; //��ѯ���

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
	for (; *instr; instr++) {
		retval = 10 * retval + (*instr - '0');
	}
	return retval;
}

/***************************���ݿ����***********************************/
//��ʵ���е����ݿ����������д��sql��䣬Ȼ����mysql_query(&mysql,query)����ɣ������������ݿ�����ɾ�Ĳ�
//��ѯ����
set<long long> QueryBlack(bool group)
{
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		set<long long> l1;
		char *str = new char[20];
		if (group) {
			sprintf(query, "select GroupId from blackList"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		}
		else {
			sprintf(query, "select QQId from blackQQ"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
		}
		mysql_query(con, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
		//����0 ��ѯ�ɹ�������1��ѯʧ��
		mysql_query(con, query);
		res = mysql_store_result(con);
		//��ȡ�ֶε���Ϣ
		char *str_field[32];  //����һ���ַ�������洢�ֶ���Ϣ
		str_field[1] = mysql_fetch_field(res)->name;
		while (column = mysql_fetch_row(res))   //����֪�ֶ���������£���ȡ����ӡ��һ��
		{
			l1.insert(my_atoll(column[0]));  //column��������
		}
		mysql_free_result(res);
		mysql_close(con);
		return l1;
	}
}

//��������
bool InsertBlack(long long blackId,bool group)
{
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		con->reconnect = 1;
		if (group) {
			sprintf(query, "insert into blackList(groupId) values ('%s');", lltoString(blackId).c_str());  //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
		}
		else {
			sprintf(query, "insert into blackQQ(QQId) values ('%s');", lltoString(blackId).c_str());  //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
		}
		if (mysql_query(con, query))        //ִ��SQL���
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
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		con->reconnect = 1;
		if (group) {
			sprintf(query, "delete from blackList where groupId='%s';", lltoString(blackId).c_str());
		}
		else {
			sprintf(query, "delete from blackQQ where QQId='%s';", lltoString(blackId).c_str());
		}
		if (mysql_query(con, query))        //ִ��SQL���
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
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		con->reconnect = 1;
		sprintf(query, "update user set email='lilei325@163.com' where name='Lilei'");
		if (mysql_query(con, query))        //ִ��SQL���
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