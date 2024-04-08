# 分布式集群聊天服务器

**Todo List**
1. - [√] 支持多机扩展（负载均衡）
2. - [√] 服务器的消息通信
3. - [×] 采用数据库连接池
4. - [×] 服务器-客户端心跳机制


## nginx配置tcp负载均衡
需要一个拿来即用的负载均衡器，在nginx1.9版本之前，只支持http协议web服务器的负载均衡器，1.9版本之后，支持tcp长连接负载均衡器，但是nginx默认编译不带tcp负载均衡模块，编译时需要带上： --with-stream.
即解压nginx包之后， 
```
./configure --with-stream   
编译安装 make && make install
文件保存在/usr/local/nginx中，配置文件在conf目录下
```
**配置tcp负载均衡**
修改nginx.conf配置文件，客户端向8000端口发数据 权重1：1派发任务用轮询机制。
修改完成后/sbin目录下启动  可以通过netstat -tanp查看端口信息
./nginx -s reload  平滑启动配置
```
# nginx tcp loadbalance config
stream{
    upstream MyServer {
        server 127.0.0.1:9999 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:9997 weight=1 max_fails=3 fail_timeout=30s;
    }
    server {
        proxy_connect_timeout 1s;
        #proxy_timeout 3s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}
因此启动配置项的客户端（对应ip和端口）后，客户端连接8000端口即可，负载均衡器轮询分发任务。
```

## 服务器中间件--基于发布-订阅的Redis
类似MQTT协议的发布-订阅 subscribe--publish

## redis发布-订阅客户端
redis支持多种不同的客户端编程语言（Java : jedis , php : phpredis, C++ : hiredis）
从github下载hiredis客户端，进行源码编译安装
```
git clone https://github.com/redis/hiredis
cd hiredis
make
sudo make install
```


### redis环境安装和配置
```
sudo apt-get install redis-server
```
安装完redis会自动启动redis服务，通过ps命令确认：
```
ps -ef | grep redis
```





## 数据格式
采用json封装数据，每个数据包必须带有msgid。
对于登录、注册、添加好友、创建群聊、添加群聊的消息，服务器会进行消息响应。
对于聊天消息（一对一、群聊、离线消息），服务器直接转发。


**msgid**
| 枚举类型 | int | 说明 |
|:------|:-------:|------:|
| LOGIN_MSG | 1 | 登录消息 |
| LOGIN_MSG_ACK | 2 | 登录响应消息 |
| REG_MSG | 3 | 注册消息 |
| REG_MSG_ACK | 4 | 注册响应 |
| ONE_CHAT_MSG | 5 | 聊天消息 私聊 |
| ADD_FRIEND_MSG | 6 | 添加好友消息 |
| ADD_FRIEND_ACK | 7 | 添加好友 回应 |
| CREATE_GROUP_MSG | 8 | 创建群组 |
| CREATE_GROUP_ACK | 9 | 创建群组响应 |
| ADD_GROUP_MSG | 10 | 加入群组 |
| ADD_GROUP_ACK | 11 | 加入群组响应 |
| GROUP_CHAT_MSG | 12 | 群聊天 |
| LOGINOUT_MSG | 13 | 用户退出登录 |


**ErrorNum**
| 枚举类型 | int | 说明 |
|:------|:-------:|------:|
| Ack_OK | 0 | 正常返回0 成功 |
| NO_COUNT_ERROR | 1 | 没有该用户 |
| INFO_ERROR | 2 | 账户或密码错误 |
| ONLINE_ERROR | 3 | 已经登录 |
| REG_ERROR | 4 | 注册失败 |
| CREATE_GROUP_ERROR | 5 | 创建群聊失败 |
| ADD_FRIEND_ERROR | 6 | 添加好友失败 |
| ADD_GROUP_ERROR | 7 | 添加群聊失败 |
