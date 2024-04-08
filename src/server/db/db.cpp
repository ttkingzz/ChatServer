#include "db.h"

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "debian-sys-maint";
static string password = "7aQhYWR7W75iCO0T";
static string dbname ="chat";

MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
MySQL::~MySQL()
{
    if(_conn!=nullptr)
        mysql_close(_conn);
}
// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn,server.c_str(),user.c_str(),
    password.c_str(),dbname.c_str(),3306,nullptr,0);
    if(p != nullptr)
    {
        // C C++默认ASCII 不设置的话MYSQL上拉取下来中文为?
        mysql_query(_conn,"set names dgk");
        return true;
    }
    // return p;
    return false;
}

// 更新操作
bool MySQL::update(string sql)
{
    if(mysql_query(_conn,sql.c_str()))
    {
        // 增删改 失败
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES* MySQL::query(string sql)
{
    if(mysql_query(_conn,sql.c_str()))
    {
        // 查询失败
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL* MySQL::getConnection()
{
    return this->_conn;
}