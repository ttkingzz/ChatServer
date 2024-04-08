#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;


// User表的增加方法
bool UserModel::insert(User &user)
{
    // 1.组装SQL语句
    char sql[1024] = {0};
    sprintf(sql,"insert into User(name,password,state) values('%s','%s',%d)",
    user.getName().c_str(),user.getPassword().c_str(),user.getState()
    );

    std::cout << sql << std::endl;

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql)){
            // 获取插入成功的用户数据生成的主键 ID---用户号
            user.setID(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
// {"msgid":2,"name":"wangwu","password":"123456"}


User UserModel::query(int id)
{
    char sql[1024]={0};
    sprintf(sql,"select * from User where id=%d",id);

    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                User user;
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                if( string(row[3]) == "online")
                    user.setState(ONLINE);
                else    
                    user.setState(OFFLINE);
                mysql_free_result(res);  //释放资源！！
                return user;
            }
        }
    }
    return User();
}


bool UserModel::updateState(User user)
{
    char sql[1024] = {0};
    // update User set state = %d where id = %d
    sprintf(sql,"update User set state = %d where id = %d",user.getState(),user.getID());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.query(sql))
        {
            return true;
        }
    }
    return false;
}

void UserModel::resetState()
{
    string sql = "update User set state=2";  // where state=online
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql.c_str());
    }
}