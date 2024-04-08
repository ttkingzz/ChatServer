#include "chatservice.hpp"
#include "public.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <vector>
using namespace std;
using namespace muduo;
ChatService* ChatService::instance()
{
    static ChatService service;  // 单例对象 线程安全
    return &service;
}

// 注册消息以及对应的回调操作 Handler   绑定器+回调
ChatService::ChatService()
{
    // 登录 注册 退出
    _msgHandlerMap.insert( {LOGIN_MSG, bind(&ChatService::login,this,placeholders::_1 ,placeholders::_2,placeholders::_3) });   
    _msgHandlerMap.insert( {REG_MSG,   bind(&ChatService::reg,  this,placeholders::_1 ,placeholders::_2,placeholders::_3) });
    _msgHandlerMap.insert( {LOGINOUT_MSG,   bind(&ChatService::userLoginout,  this,placeholders::_1 ,placeholders::_2,placeholders::_3) });

    // 私聊 添加朋友
    _msgHandlerMap.insert( {ONE_CHAT_MSG, bind(&ChatService::oneChat,  this,placeholders::_1 ,placeholders::_2,placeholders::_3) });
    _msgHandlerMap.insert( {ADD_FRIEND_MSG, bind(&ChatService::addFriend,  this,placeholders::_1 ,placeholders::_2,placeholders::_3)} );

    // 创建群 添加群 群聊
    _msgHandlerMap.insert( {CREATE_GROUP_MSG, bind(&ChatService::createGroup,  this,placeholders::_1 ,placeholders::_2,placeholders::_3)} );
    _msgHandlerMap.insert( {ADD_GROUP_MSG, bind(&ChatService::addGroup,  this,placeholders::_1 ,placeholders::_2,placeholders::_3)} );
    _msgHandlerMap.insert( {GROUP_CHAT_MSG, bind(&ChatService::groupChat,  this,placeholders::_1 ,placeholders::_2,placeholders::_3)} );

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }


}

/*==============错误消息===============*/
MsgHalder ChatService::getHandler(int msgID)
{
    // 记录错误日志 msgID无对应的事件处理
    auto it = _msgHandlerMap.find(msgID);
    if(it == _msgHandlerMap.end())
    {   
        // 返回一个默认的处理器 空操作
        return [=](const TcpConnectionPtr &conn, json &js,Timestamp time){
            LOG_ERROR << "msgid : " << msgID << " can not find handler.";  //自动输出endl
        };
    }
    else{
        return _msgHandlerMap[msgID]; 
    }
    // return _msgHandlerMap[msgID]; 不要用中括号查询判断 查不到会添加
}

/*======================登录====================*/
// 处理登录逻辑 id pwd 是否正确 返回值
void ChatService::login(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if(user.getID() == id && user.getPassword() == pwd)
    {
        // 如果已经onlie 不允许重复登录
        if(user.getState() == ONLINE)
        {
            json responce;
            responce["msgid"] = LOGIN_MSG_ACK; 
            responce["errno"] = ONLINE_ERROR;
            responce["errmsg"] = "This user is already online";
            conn->send(responce.dump());
        }else{
            // 登录成功 更新用户状态信息  state offline--> onlie
            {
                lock_guard<mutex> lock(_connMutex);  // lock_guard不支持手动解锁，但是比unique_lock轻量
                _connMap.insert({id, conn});  //确保线程安全
            }

            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 

            user.setState(ONLINE);
            _userModel.updateState(user); //刷新数据并返回
            json responce;
            responce["msgid"] = LOGIN_MSG_ACK; 
            responce["errno"] = Ack_OK;
            responce["id"] = user.getID();
            responce["name"] = user.getName();
            
            // 查询用户是否有离线消息 
            vector<string> offMsgVec = _offMsgModel.query(id);
            if(!offMsgVec.empty())
            {
                responce["offlinemsg"] = offMsgVec;  // 这个json库可以直接转换
                // 拿出消息了 记得删除
                _offMsgModel.remove(id);
            }

            // 登录成功 返回好友列表
            vector<User> friendInfo = _friendModel.query(id);
            if(!friendInfo.empty())
            {
                // responce["friends"] = friendInfo; // 只支持基础数据类型
                vector<string> temp_friend;
                for(User &user:friendInfo)
                {
                    json friendJs;
                    friendJs["id"] = user.getID();
                    friendJs["name"] = user.getName();
                    friendJs["state"] = user.getState();
                    temp_friend.push_back(friendJs.dump());
                }
                responce["friends"] = temp_friend;
            }

            // 查询是否有加入群聊
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getID();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                responce["groups"] = groupV;
            }
            conn->send(responce.dump());
        }

    }
    else{
        json responce;
        if(user.getID() == -1)
        {
            responce["msgid"] = LOGIN_MSG_ACK; 
            responce["errno"] = NO_COUNT_ERROR;
            responce["errmsg"] = "can not find this count";
            conn->send(responce.dump());
        }
        // 该用户不存在登陆失败
        else{
            responce["msgid"] = LOGIN_MSG_ACK; 
            responce["errno"] = INFO_ERROR;
            responce["errmsg"] = "user name or password is wrong";
            conn->send(responce.dump());
        }
    }
}

/*=========================注册===========================*/
// 处理注册业务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    // 解析收到的数据
    string name = js["name"];
    string pwd = js["password"];
    LOG_INFO << js.is_string();
    User user;
    user.setName(name);
    user.setPassword(pwd);
    bool state = _userModel.insert(user);  // 插入数据库
    if(state)
    {
        // 注册成功
        json responce;
        responce["msgid"] = REG_MSG_ACK; // 注册返回 3
        responce["errno"] = Ack_OK;
        responce["id"] = user.getID();
        conn->send(responce.dump());
    }else{
        // 注册失败
        json responce;
        responce["msgid"] = REG_MSG_ACK;
        responce["errno"] = REG_ERROR;
        conn->send(responce.dump());
    }
}

/*=================用户退出==================*/
//用户正常退出
void ChatService::userLoginout(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    int userId = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _connMap.find(userId);
        if(it != _connMap.end())
        {
            _connMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userId); 

    User user(userId,"","",2);
    _userModel.updateState(user);
}

// 处理用户异常退出
void ChatService::clientClosedException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(mutex);
        for(auto it = _connMap.begin();it!= _connMap.end();++it)
        {
            if(it->second==conn)
            {
                user.setID(it->first);
                _connMap.erase(it);
                break;
            }
        }
    }
    
    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getID()); 

    if(user.getID() != -1)
    {
        user.setState(OFFLINE);
        _userModel.updateState(user);   // 根据id更改state
    }
}
/*===================用户退出end=================*/


/*====================一对一聊天==========================*/
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    // 哦 msgid是我的，解析json吧
    // {"msgid":  , "id":  ,"name": , "to":  , "msg:"  }
    LOG_INFO << js.dump();
    int toid = js["toid"].get<int>();
    // int localId = js["id"].get<int>();
    
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _connMap.find(toid);  // _connMap存储的是所有在线用户的信息
        if(it!=_connMap.end())
        {
            // 肯定对方在线 我给你发消息啦
            // 服务器主动推送消息  conn->send
            it->second->send(js.dump());    // conn->send() 服务站主动推送消息
            return ;  //返回后 lock出作用域 析构
        }
    }

    // 查询是否数据库中在线 
    User user = _userModel.query(toid);
    if(user.getState() != OFFLINE)
    {
        _redis.publish(toid,js.dump());  //告诉其他的服务器 你有消息
        return;
    }


    _offMsgModel.insert(toid,js.dump());   // 注意插入offMsg数据库中 todi
}


/*===============群相关====================*/
// 创建群
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    LOG_INFO << "create gourp " << js.dump();
    int userid = js["id"].get<int>();
    string gName = js["groupname"];
    string gDesc = js["groupdesc"];
    Group group(-1,gName,gDesc);
    json responce;
    if(_groupModel.createGroup(group))   // 存入AllGroup表中
    {
        // 群创建成功 这个人的id 创建的群id存入GroupUser表中
        _groupModel.addGroup(userid,group.getId(),"creator");
        responce["msgid"] = CREATE_GROUP_ACK;
        responce["errno"] = Ack_OK;
        responce["groupid"] = group.getId();
    }
    else  //创建群组失败
    {  
        responce["msgid"] =  CREATE_GROUP_ACK; 
        responce["errno"] = CREATE_GROUP_ERROR;
        responce["errmsg"] = "this group name already exit...!" ;
    }
    conn->send(responce.dump());
}


void ChatService::addGroup(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    
    Group group = _groupModel.queryGroupEixt(groupId);  // 查一下数据库 是否存在groupid
    json response;
    if(group.getId() != -1)
    {
        _groupModel.addGroup(userId,groupId,"normal");
        response["msgid"] =  ADD_GROUP_ACK; 
        response["errno"] = Ack_OK;   
        response["groupname"] = group.getName();
        response["groupdesc"] = group.getDesc();
        /*业务需求：是否返回群里面人员信息*/
        
    }
    else
    {
        response["msgid"] =  ADD_GROUP_ACK; 
        response["errno"] = ADD_GROUP_ERROR;
        response["errmsg"] = "this group does not exit...!";
    }
    conn->send(response.dump());
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> userIdVec = _groupModel.queryGroupUsers(userid,groupid);

    lock_guard<mutex> lock(_connMutex);
    for(int id:userIdVec)   // 拿到每个群成员的id
    {
        auto it = _connMap.find(id);
        if(it!=_connMap.end())
        {
            it->second->send(js.dump());
        }
        else{
        // 查一下数据库 是否用户online
            User user = _userModel.query(id);
            if(user.getState() != OFFLINE)
            {
                _redis.publish(id,js.dump());
            }
            else{
                _offMsgModel.insert(id,js.dump());
            }
            
        }
    }
}
/*==================群end==========================*/


/*===================添加好友=======================*/
 // 添加好友 msgdi id friendid  登录时需要给出好友列表
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    User user = _userModel.query(friendid);
    json responce;
    if(user.getID() != -1)
    {
        _friendModel.insert(userid,friendid);
        // 添加成功 返回好友信息
        
        responce["msgid"] = ADD_FRIEND_ACK; 
        responce["errno"] = Ack_OK;
        responce["id"] = user.getID();
        responce["name"] = user.getName();
        responce["state"] = user.getState();
    }
    else{
        // 没有这个好友信息啊
        responce["msgid"] = ADD_FRIEND_ACK; 
        responce["errno"] = ADD_FRIEND_ERROR;
        responce["msg"] = "can not find this friend";
    }
    conn->send(responce.dump());
    // 添加成功了 也可以返回好友信息
}



/*=================服务器端异常处===============================*/
void ChatService::rest()
{
    // 把online状态的用户 设置为offline
    _userModel.resetState();
}


/*==================redis消息队列=============*/
// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _connMap.find(userid);
    if (it != _connMap.end())
    {
        it->second->send(msg);
        return;
    }
    // 存储该用户的离线消息  可能这哈用户刚好退下
    _offMsgModel.insert(userid, msg);
}