#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <functional>
#include<iostream>
using namespace std;
using namespace nlohmann;


ChatServer::ChatServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const string & nameArg)
            :_server(loop,listenAddr,nameArg),_loop(loop)
    {
        // 连接的创建和断开的回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection,this,placeholders::_1));
        // 用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage,this,placeholders::_1,placeholders::_2,placeholders::_3));
        _server.setThreadNum(4); // 一个IO线程 三个工作线程
    }


void ChatServer::start()
{
    _server.start();
}


void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // if(conn->connected())
    // {
    //     cout << conn->peerAddress().toIpPort() << " ---> " << conn->localAddress().toIpPort() << endl;
    // }else{
    //     cout << conn->peerAddress().toIpPort() << " is closed..." << endl;
    // }
    if(!conn->connected())
    {
        // 客户端异常关闭
        ChatService::instance()->clientClosedException(conn);
        conn->shutdown();
    }
}
// 专门处理用户的读写事件  
void ChatServer::onMessage(const TcpConnectionPtr &conn,  // 连接
                        Buffer *buf,         // 缓冲区
                        Timestamp time)       // 接收到数据的时间信息
{
    string buffer = buf->retrieveAllAsString();
    if(buffer.find("msgid") == string::npos) return;  //过滤消息
    // cout << "receiv : " << buffer << "time: " << time.toString() << endl;
    // conn->send(buffer);
    json js = json::parse(buffer);  // 数据解码 反序列化
    // 网络模块 千万不要调用业务模块

    // 完全解耦网络模块代码和业务模块代码
    // js["msgid"]绑定回调操作  获取----业务handler --conn js time
    // 是否需要先判断js中是否有msgid   根据msgid判断消息处理回调：
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());  // 注意如果没有msgid 会导致core dumped
    // 绑定好的事件处理器 执行响应的业务处理
    msgHandler(conn,js,time); 
}
