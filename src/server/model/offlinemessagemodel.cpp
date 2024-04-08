#include "offlinemessagemodel.hpp"
#include "db.h"

bool OfflineMsgModel::insert(int userid, string msg)
{
    char sql[1024] = {0};
    sprintf(sql,"insert into OfflineMessage value(%d,'%s')",userid,msg.c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
            return true;
    }
    return false;
}

// 删除用户的离线消息
bool OfflineMsgModel::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql,"delete from OfflineMessage where userid=%d",userid);
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
            return true;
    }
    return false;
}

// 查询用户的离线消息 不止一条
vector<string> OfflineMsgModel::query(int userid)
{
    char sql[1024] = {0};
    sprintf(sql,"select message from OfflineMessage where userid=%d",userid);
    
    vector<string> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;  // 返回empty
}