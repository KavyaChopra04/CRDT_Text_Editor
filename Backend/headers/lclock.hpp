#ifndef LCLOCK_H
#define LCLOCK_H

#include <map>
#include <string>

class LClock {
public:
    LClock(const std::string& id);
    void process_time(int value);
    LClock& advance(const LClock& other);
    int accumulate() const;

private:
    std::string id;
    std::map<std::string, int> curr_time;
};

#endif // LCLOCK_H
