// libmuduo_base.so   libmuduo_net.so libpthread.so

/*
TcpServer
TcpClient
把网络I/O代码和业务代码区分开
        用户的连接和断开 用户的可读写事件
        什么时候发生 读写、监听不用管了
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <iostream>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;

/*
g++ muduo_test.cpp -lmuduo_net -lmuduo_base -lpthread
组合TcpServer对象
创建EvnetLoop事件循环对象的指针
明确TcpServer的构造函数
当前服务器类的构造函数。注册处理连接的回调函数 和处理读写的回调
添加线程 自动分离
*/

class ChatServer
{
public:
    ChatServer(EventLoop *loop,
                const InetAddress &listenAddr,  // IP+port
                const string &nameArg   //服务器名字
                ) 
            :_server(loop,listenAddr,nameArg),_loop(loop)
    {
        // 连接的创建和断开的回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection,this,placeholders::_1));
        // 用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage,this,placeholders::_1,placeholders::_2,placeholders::_3));
        _server.setThreadNum(4); // 一个IO线程 三个工作线程
    }

    void start()
    {
        _server.start();
    }
private:
    // 用于处理用户连接和断开
    void onConnection(const TcpConnectionPtr &conn)
    {
        if(conn->connected())
        {
            // toIp()  toPort() toIpPort() 
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << endl;
        }
        else{
            cout << "close" << endl;
            // 服务器断开； 连接断开
            conn->shutdown();  //close(fd);
            // _loop->quit();  退出服务

        }

    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn,  // 连接
                            Buffer *buf,         // 缓冲区
                            Timestamp time)       // 接收到数据的时间信息
    {
        string buffer = buf->retrieveAllAsString();
        cout <<  "recv data:" << buffer << "time:" << time.toString() << endl;

        conn->send(buf);
    }
    TcpServer _server;
    EventLoop *_loop;  //epoll
};

int main()
{
    EventLoop loop;  // epoll
    InetAddress addr("127.0.0.1",9999);
    ChatServer server(&loop,addr,"chatserver");
    server.start(); // listen fd 添加到epoll 上
    loop.loop();  // 以阻塞的方式等待新用户连接 读写事件等操作

    return 0;
}