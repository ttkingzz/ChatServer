#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include "groupmodel.hpp"
#include <string>
#include <vector>

class GroupModel
{
public:
    // 创建群聊
    bool createGroup(Group &group);
    // 加入群聊
    void addGroup(int userid,int groupid,string role="normal");
    // 查询用户所在群组信息 加了什么群
    vector<Group> queryGroups(int userid);
    // 根据groupid查群组用户列表  用于群聊  发给在线or离线
    vector<int> queryGroupUsers(int userid,int groupid);
    Group queryGroupEixt(int groupDd);
};

#endif