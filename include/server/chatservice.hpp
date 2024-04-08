#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"
using namespace nlohmann;
using namespace muduo;
using namespace muduo::net;
using namespace std;
#include <functional>
#include <mutex>


#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
// 表示处理消息的事件  回调方法类型
using MsgHalder = std::function<void(const TcpConnectionPtr &conn, json &js,Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService* instance();
    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js,Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js,Timestamp time);

    // 一对一 私聊
    void oneChat(const TcpConnectionPtr &conn, json &js,Timestamp time);
    
    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js,Timestamp time);

    // 处理用户异常退出 传入退出的连接信息
    void clientClosedException(const TcpConnectionPtr &conn);

    // 创建群
    void createGroup(const TcpConnectionPtr &conn, json &js,Timestamp time);
    void addGroup(const TcpConnectionPtr &conn, json &js,Timestamp time);
    void groupChat(const TcpConnectionPtr &conn, json &js,Timestamp time);

    // 用户正常退出
    void userLoginout(const TcpConnectionPtr &conn, json &js,Timestamp time);

    // 服务器异常关闭 数据库内存重置
    void rest();
    // 获取消息对应的处理器
    MsgHalder getHandler(int msgID);

    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);
private:
    ChatService();

    // 存储消息id 以及对应的业务处理方法
    unordered_map<int,MsgHalder> _msgHandlerMap;

    // 每个上线的用户进行存储  服务器要主动的推送消息
    unordered_map<int,TcpConnectionPtr> _connMap;

    // 服务器在线用户Map的mutex
    mutex _connMutex;


    // 操作数据库的相关方法
    // 数据库相关的操作都在model中
    UserModel _userModel;

    // 数据库 离线消息
    OfflineMsgModel _offMsgModel;

    // 好友列表
    FriendMOdel _friendModel;

    // 群组
    GroupModel _groupModel;

    
    // redis操作对象
    Redis _redis;
};


#endif