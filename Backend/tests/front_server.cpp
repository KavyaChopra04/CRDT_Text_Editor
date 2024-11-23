#include <pthread.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <queue>
#include "../headers/commands.hpp"
#include "../headers/ConcurrentLL.hpp"
//#include "lclock.hpp"
using namespace std;


queue<Command> CQ;
//ConcurrentLinkedList state;



void* FrontConnect(void* arg){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    string name = "alice";
    int clock = 0;
    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, 1);

    // accepting connection request
    int clientSocket
        = accept(serverSocket, nullptr, nullptr);

    // recieving data

    while(clientSocket){
    char buffer[2048] = { 0 };
     int status = recv(clientSocket, buffer,10, 0);
     cout << buffer << endl;
     if(status == 0){break;}
     stringstream ss(buffer);
     string word;
     ss >> word;
     if(word == "INSERT"){
        ss >> word;
        int index = stoi(word);
        ss >> word;
        char data = word[0];
        CQ.push(Insert(index,data,"alice",clock));
        clock++;
        continue;
     }
     if(word == "DELETE"){
        ss >> word;
        int index = stoi(word);
        CQ.push(Delete(index));
        continue;
     }
    
    }
    // closing the socket.
    close(serverSocket);
    return NULL;
}

int main(){
  pthread_t pid;
  pthread_create(&pid,NULL,&FrontConnect,NULL);
  pthread_join(pid,NULL);
  return 0;
}