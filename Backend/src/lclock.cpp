#include <map>
#include <string>
#include <stdexcept>
#include "headers/gcounter.h"


LClock::LClock(const std::string& id){
   this->id = id;
   this->curr_time[id] = 0;
}

void LClock::process_event(int value){

    this->curr_time[this->id] += value;
}

int LClock::accumulate(){
   int answer = 0;
   for(auto element: this->curr_time){
          answer += element.second;
   }
   return answer;
}

 LClock& LClock::advance(const LClock& other){

    for(auto element:other->curr_time){
        int cand = 0;
        if(this->curr_time.find(element.first) != this->curr_time.end()){
         cand = this->curr_time[element.first];
        }
        this->curr_time[element.first] = max(cand,element.second);
    }
   return *this;
 }