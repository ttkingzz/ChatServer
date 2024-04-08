#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;
// 聊天服务器
class ChatServer
{
public:
    ChatServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const string & nameArg);
    void start();
private:
    TcpServer _server;  //组合的muduo库的实现
    EventLoop *_loop;
    void onConnection(const TcpConnectionPtr &conn);
    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn,  // 连接
                            Buffer *buf,         // 缓冲区
                            Timestamp time);       // 接收到数据的时间信息
};

#endif