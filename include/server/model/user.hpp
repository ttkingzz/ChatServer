#ifndef USER_H
#define USER_H
#include "public.hpp"
#include <string>
using namespace std;
// User表 
// 匹配user表的映射关系(ORM)类
// 1 online 2 offiline
class User
{
public:
    User(int id=-1,string name="",string pwd="",int state=OFFLINE)
    {   
        this->id = id;
        this->name = name;
        this->password =password;
        this->state = state;
    }
    void setID(int id){this->id=id;};
    void setName(string name){this->name=name;};
    void setPassword(string password){this->password=password;};
    void setState(int state){this->state=state;};

    int getID(){return this->id;};
    string getName(){return this->name;};
    string getPassword(){return this->password;};
    int getState(){return this->state;};
protected:
    int id;
    string name;
    string password;
    int state;
};

#endif