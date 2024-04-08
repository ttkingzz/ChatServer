#ifndef GROUP_H
#define GROUP_H
#include "groupuser.hpp"
#include <vector>
#include <string>
using namespace std;

class Group{
public:
    Group(int id=-1,string name="",string desc="")
    {
        this->id=id;
        this->groupdesc =desc;
        this->groupname = name;
    }
    void setId(int id){this->id = id;};
    void setName(string name){this->groupname=name;};
    void setDesc(string desc){this->groupdesc=desc;};

    int getId(){return this->id;};
    string getName(){return this->groupname;};
    string getDesc(){return this->groupdesc;};
    vector<GroupUser> &getUsers(){return this->groupUsers;};

private:
    int id;
    string groupname;
    string groupdesc;
    vector<GroupUser> groupUsers;   // 组里的所有成员；
};

#endif