#include <stdio.h>
#include <WinSock.h>  //一定要包含这个，或者winsock2.h
#include "mysql.h"    //引入mysql头文件(一种方式是在vc目录里面设置，一种是文件夹拷到工程目录，然后这样包含)
#include <Windows.h>
#include <string>
#include "MysqlClient.h"
#include <strstream>
#include <sstream>
#include <set>

using namespace std;
//包含附加依赖项，也可以在工程--属性里面设置
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL_FIELD *fd;  //字段列数组
char field[32][32];  //存字段名二维数组
MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
char query[150]; //查询语句

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

/***************************数据库操作***********************************/
//其实所有的数据库操作都是先写个sql语句，然后用mysql_query(&mysql,query)来完成，包括创建数据库或表，增删改查
//查询数据
set<long long> QueryBlack(bool group)
{
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		set<long long> l1;
		char *str = new char[20];
		if (group) {
			sprintf(query, "select GroupId from blackList"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
		}
		else {
			sprintf(query, "select QQId from blackQQ"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
		}
		mysql_query(con, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码
		//返回0 查询成功，返回1查询失败
		mysql_query(con, query);
		res = mysql_store_result(con);
		//获取字段的信息
		char *str_field[32];  //定义一个字符串数组存储字段信息
		str_field[1] = mysql_fetch_field(res)->name;
		while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行
		{
			l1.insert(my_atoll(column[0]));  //column是列数组
		}
		mysql_free_result(res);
		mysql_close(con);
		return l1;
	}
}

//插入数据
bool InsertBlack(long long blackId,bool group)
{
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		con->reconnect = 1;
		if (group) {
			sprintf(query, "insert into blackList(groupId) values ('%s');", lltoString(blackId).c_str());  //可以想办法实现手动在控制台手动输入指令
		}
		else {
			sprintf(query, "insert into blackQQ(QQId) values ('%s');", lltoString(blackId).c_str());  //可以想办法实现手动在控制台手动输入指令
		}
		if (mysql_query(con, query))        //执行SQL语句
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


//删除数据
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
		if (mysql_query(con, query))        //执行SQL语句
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

//修改数据
bool ModifyData()
{
	MYSQL* con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0)) {
		con->reconnect = 1;
		sprintf(query, "update user set email='lilei325@163.com' where name='Lilei'");
		if (mysql_query(con, query))        //执行SQL语句
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