#include <stdio.h>
#include <WinSock.h>  //一定要包含这个，或者winsock2.h
#include "mysql.h"    //引入mysql头文件(一种方式是在vc目录里面设置，一种是文件夹拷到工程目录，然后这样包含)
#include <Windows.h>
#include <string>
#include <list>

using namespace std;

//包含附加依赖项，也可以在工程--属性里面设置
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL mysql; //mysql连接
MYSQL_FIELD *fd;  //字段列数组
char field[32][32];  //存字段名二维数组
MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
char query[150]; //查询语句

bool ConnectDatabase();     //函数声明
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

//连接数据库
bool ConnectDatabase()
{
	//初始化mysql
	mysql_init(&mysql);  //连接mysql，数据库

	//返回false则连接失败，返回true则连接成功
	if (!(mysql_real_connect(&mysql, "123.207.150.160", "root", "root", "Dice", 3306, NULL, 0))) //中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
	{
		return false;
	}
	else
	{
		return true;
	}
}
//释放资源
void FreeConnect()
{
	//释放资源
	mysql_free_result(res);
	mysql_close(&mysql);
}
/***************************数据库操作***********************************/
//其实所有的数据库操作都是先写个sql语句，然后用mysql_query(&mysql,query)来完成，包括创建数据库或表，增删改查
//查询数据
std::list<std::string> QueryBlack()
{
	list<string> l1;
	sprintf(query, "select GroupId from blackList"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
	mysql_query(&mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码
	//返回0 查询成功，返回1查询失败
	mysql_query(&mysql, query);
	res = mysql_store_result(&mysql);
	//获取字段的信息
	char *str_field[32];  //定义一个字符串数组存储字段信息
	str_field[1] = mysql_fetch_field(res)->name;
	while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行
	{
		l1.push_back(column[0]);  //column是列数组
	}
	return l1;
}

//插入数据
bool InsertData()
{
	sprintf(query, "insert into user values (NULL, 'Lilei', 'wyt2588zs','lilei23@sina.cn');");  //可以想办法实现手动在控制台手动输入指令
	if (mysql_query(&mysql, query))        //执行SQL语句
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
//修改数据
bool ModifyData()
{
	sprintf(query, "update user set email='lilei325@163.com' where name='Lilei'");
	if (mysql_query(&mysql, query))        //执行SQL语句
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