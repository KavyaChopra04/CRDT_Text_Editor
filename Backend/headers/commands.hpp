#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <string>
#include "ConcurrentLL.hpp"

enum Comm_types{
    INSERT,
    DELETE,
    MERGE
};

class Command {
public:
    Command(enum Comm_types command);
    enum Comm_types this_type();

protected:
    enum Comm_types my_type;
};

class Insert : public Command{

    public:
    Insert(int index, char payload , std::string uuid, long long stamp);
    CausalDot ts;
    int index;
    char payload;
};

class Delete : public Command{

    public:
    Delete(int index);
    int index;
};

class Merge : public Command{
    public:
    Merge(std::vector<Node> newList);
    std::vector<Node> otherList;
};

#endif // LCLOCK_H