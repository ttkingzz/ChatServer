#include <iostream>
#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>
using namespace std;

// ORM 对象关系映射，业务层看不到任何sql语句，
// 网络层 业务层 数据层  不要掺杂
// muduo  逻辑业务  数据库
 
void resetHandler(int)  // ctrl+c导致服务器挂
{
    ChatService::instance()->rest();
    exit(0);
}
int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "invalid command! example: ./ChatServer 127.0.0.1 9999" << endl;
        exit(-1);
    }
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress listenAddr(ip,port);
    ChatServer chat(&loop,listenAddr,"chat");
    chat.start();
    loop.loop(); 
    return 0;
}