#include "../headers/commands.hpp"
#include "../headers/concurrentLL.hpp"
Command::Command(enum Comm_types command){
    this->my_type = command;
}

enum Comm_types Command::this_type(){
    return this->my_type;
}

Insert::Insert(int index, char payload , std::string uuid, long long stamp): ts(CausalDot(uuid,stamp)),index(index),payload(payload) ,Command(INSERT){}

Delete::Delete(int index): index(index),Command(DELETE){}

Merge::Merge(std::vector<Node> newList): otherList(newList), Command(MERGE){}