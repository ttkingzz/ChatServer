#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;
void func1()
{
    json js;
    js["msg_type"]=2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello";
    std::cout << js << std::endl;

    string sendBuf = js.dump(); //dump发送
    // cout << sendBuf << endl;
    cout << sendBuf.c_str() << endl;
}

void func2()
{
    json js;
    js["id"] = {1,2,3,4};
    js["msg"]["zhang ssan"] = "hello";  
    js["msg"]["li si"] = "hi";    // msg 键下对应的两个k-v
    cout << js << endl;
}


string func3()
{
    vector<int> v;
    v.emplace_back(1);
    v.emplace_back(7);
    v.emplace_back(3);

    map<int,string> m;
    m.insert({3,"tom"});
    m.insert({2,"jerry"});
    m.insert({5,"jack"});

    json js;
    js["vector"] = v;
    js["map"] = m;
    cout << js << endl;

    string sendbuf = js.dump();
    return sendbuf;
}


int main()
{
    func1();
    func2();
    // 数据反序列化  json字符串--反序列化 数据对象
    string recv = func3();

    json jsbuf = json::parse(recv);

    map<int,string> myMap = jsbuf["map"];
    cout << jsbuf["map"] << endl;  //  访问key
    return 0;
}
