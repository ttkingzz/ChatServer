#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
// user表的具体操作类
class UserModel{
public:
    // User表的增加方法
    bool insert(User &user);

    User query(int id);  // 查询用户

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户状态信息
    void resetState();
};


#endif