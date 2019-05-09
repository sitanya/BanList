#include <stdio.h>
#include <WinSock.h>  //һ��Ҫ�������������winsock2.h
#include "mysql.h"    //����mysqlͷ�ļ�(һ�ַ�ʽ����vcĿ¼�������ã�һ�����ļ��п�������Ŀ¼��Ȼ����������)
#include <Windows.h>
#include <string>
#include <list>

using namespace std;

//�������������Ҳ�����ڹ���--������������
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL mysql; //mysql����
MYSQL_FIELD *fd;  //�ֶ�������
char field[32][32];  //���ֶ�����ά����
MYSQL_RES *res; //����ṹ�������е�һ����ѯ�����
MYSQL_ROW column; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
char query[150]; //��ѯ���

bool ConnectDatabase();     //��������
void FreeConnect();
std::list<std::string> QueryBlack();
bool InsertData();
bool ModifyData();
bool DeleteData();
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

//�������ݿ�
bool ConnectDatabase()
{
	//��ʼ��mysql
	mysql_init(&mysql);  //����mysql�����ݿ�

	//����false������ʧ�ܣ�����true�����ӳɹ�
	if (!(mysql_real_connect(&mysql, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))) //�м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ
	{
		return false;
	}
	else
	{
		return true;
	}
}
//�ͷ���Դ
void FreeConnect()
{
	//�ͷ���Դ
	mysql_free_result(res);
	mysql_close(&mysql);
}
/***************************���ݿ����***********************************/
//��ʵ���е����ݿ����������д��sql��䣬Ȼ����mysql_query(&mysql,query)����ɣ������������ݿ�����ɾ�Ĳ�
//��ѯ����
std::list<std::string> QueryBlack()
{
	list<string> l1;
	sprintf(query, "select GroupId from blackList"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
	mysql_query(&mysql, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
	//����0 ��ѯ�ɹ�������1��ѯʧ��
	mysql_query(&mysql, query);
	res = mysql_store_result(&mysql);
	//��ȡ�ֶε���Ϣ
	char *str_field[32];  //����һ���ַ�������洢�ֶ���Ϣ
	str_field[1] = mysql_fetch_field(res)->name;
	while (column = mysql_fetch_row(res))   //����֪�ֶ���������£���ȡ����ӡ��һ��
	{
		l1.push_back(column[0]);  //column��������
	}
	return l1;
}

//��������
bool InsertData()
{
	sprintf(query, "insert into user values (NULL, 'Lilei', 'wyt2588zs','lilei23@sina.cn');");  //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
	if (mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
//�޸�����
bool ModifyData()
{
	sprintf(query, "update user set email='lilei325@163.com' where name='Lilei'");
	if (mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}