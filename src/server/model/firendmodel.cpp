#include "friendmodel.hpp"
#include "db.h"

void FriendMOdel::insert(int userid,int friendid)
{
    char sql[1024] = {0};
    sprintf(sql,"insert into Friend values(%d,%d)",userid,friendid);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
    
}

// 返回用户好友列表  friendid--查User 两个表联合查询
vector<User> FriendMOdel::query(int userid)
{
    // friend user联合查询 内连接
    // select a.id,a.name,a.state from User a inner join Friend b on b.friendid=a.id where b.userid=%d
    // select * from User, Friend where Friend.friendid = User.dd;   //如果是左/右外连接 没有匹配的补NULL
    char sql[1024]={0};
    sprintf(sql,"select a.id,a.name,a.state from User a inner join Friend b on b.friendid=a.id where b.userid=%d \
    UNION select a.id,a.name,a.state from User a inner join Friend b on b.userid=a.id where b.friendid=%d",userid,userid);
    MySQL mysql;
    vector<User> friendInfoV;
    if(mysql.connect())
    {
        MYSQL_RES*res = mysql.query(sql);

        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while( (row = mysql_fetch_row(res))!=nullptr )
            {
                User user;
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                if( string(row[2]) == "online")
                    user.setState(ONLINE);
                else    
                    user.setState(OFFLINE);
                friendInfoV.emplace_back(user);
            }
            mysql_free_result(res);
            return friendInfoV;
        }
    }
    return friendInfoV;
}