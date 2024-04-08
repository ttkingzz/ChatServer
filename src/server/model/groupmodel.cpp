#include "groupmodel.hpp"
#include "db.h"
#include <string>
#include <string.h>
#include <muduo/base/Logging.h>
// 创建群聊
bool GroupModel::createGroup(Group &group)
{
    char sql[1024]={0};
    sprintf(sql,"insert into AllGroup(groupname,groupdesc) values('%s','%s')",group.getName().c_str(),group.getDesc().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
            
    }
    return false;  // 因为groupname唯一 重复插入出错
}

// 加入群聊  需要考虑如果群聊不存在？？
void GroupModel::addGroup(int userid,int groupid,string role)
{
    char sql[1024]={0};
    int role_int = 2;
    if(role == "creator") role_int=1;
    sprintf(sql,"insert into GroupUser values(%d,%d,%d)",groupid,userid,role_int);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}


// 查询用户所在群组信息 加了什么群(群id 群名字 群描述 群里面的所有人员信息)
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024]={0};
    // 查询用户加入的所有群的信息（群id 群名称 群描述)
    sprintf(sql,"select a.id,a.groupname,a.groupdesc from AllGroup a inner join GroupUser b on a.id=b.groupid where b.userid=%d",userid);
    vector<Group> groupVec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
                // group.getUsers() // 还有一个用户信息数组 需要查询这个群里面有哪些人和他们的信息（GroupUser中有userid和群组id，User中有userid)
            }
            mysql_free_result(res);
            memset(sql,0,sizeof(sql));
        }else{
            LOG_INFO <<  " fisrt queryGroups selecting error: " << sql;
        }
        
    }
    // 查询每个群组里面的人员信息
    for(Group &group : groupVec)
    {
        sprintf(sql,"select a.id,a.name,a.state,b.grouprole from User a inner join GroupUser b on a.id=b.userid where b.groupid=%d",group.getId());
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while( (row=mysql_fetch_row(res))!=nullptr )
            {
                GroupUser user;
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                if( string(row[2]) == "online")
                    user.setState(ONLINE);
                else    
                    user.setState(OFFLINE);
                user.setRole(string(row[3]) );
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }else{
            LOG_INFO <<  " second queryGroups selecting error: " << sql;
        }
    }
    return groupVec;
}

// 根据groupid查群组用户列表  用于群聊  发给在线or离线
vector<int> GroupModel::queryGroupUsers(int userid,int groupid)
{
    char sql[1024]={0};
    sprintf(sql,"select userid from GroupUser where groupid = %d and userid != %d",groupid,userid);
    MySQL mysql;
    vector<int> idVec;
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while( (row=mysql_fetch_row(res)) != nullptr )
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}


Group GroupModel::queryGroupEixt(int groupDd)
{
    char sql[1024]={0};
    sprintf(sql,"select id,groupname,groupdesc from AllGroup where id=%d",groupDd);

    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)  // 只会有一行
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                mysql_free_result(res);
                return group;
            }
        }
    }
    return Group();
}
