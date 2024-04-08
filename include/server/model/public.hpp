#ifndef PUBLIC_H
#define PUBLIC_H

/*server和client的公共文件*/
enum EnMsgType{
    LOGIN_MSG = 1,  // 登录消息
    LOGIN_MSG_ACK,  // 登录响应消息
    REG_MSG,        // 注册消息
    REG_MSG_ACK,    //注册响应

    ONE_CHAT_MSG,   //聊天消息 私聊

    ADD_FRIEND_MSG, // 添加好友消息
    ADD_FRIEND_ACK, // 添加好友 回应

    CREATE_GROUP_MSG,  // 创建群组
    CREATE_GROUP_ACK,   // 创建群组响应
    ADD_GROUP_MSG,    // 加入群组
    ADD_GROUP_ACK,    // 加入群组响应
    GROUP_CHAT_MSG,   // 群聊天

    LOGINOUT_MSG,    // 用户退出登录
    
};

// Ack返回的数值
enum ErrorNum{
    Ack_OK,   // 正常返回0 成功
    NO_COUNT_ERROR,   //没有该用户
    INFO_ERROR, // 账户或密码错误
    ONLINE_ERROR, //已经登录

    REG_ERROR, //注册失败

    CREATE_GROUP_ERROR, //创建群聊失败

    ADD_FRIEND_ERROR,  //添加好友失败 没有这个好友的嘛

    ADD_GROUP_ERROR,  //添加群聊失败 无这个群

};

enum NetState{
    ONLINE = 1,
    OFFLINE
};


#endif