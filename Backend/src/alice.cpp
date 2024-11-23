// C++ program to show the example of server application in
// socket programming
#include <pthread.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <queue>
#include "../headers/ConcurrentLL.hpp"
#include "../headers/commands.hpp"
//#include "lclock.hpp"
using namespace std;


queue<Command*> CQ;
ConcurrentLinkedList state;



void* FrontConnect(void* arg){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    string name = "alice";
    long long clock = 0;
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
     if(status == 0){break;}
     stringstream ss(buffer);
     cout << buffer << endl;
     string word;
     ss >> word;
     if(word == "INSERT"){
        ss >> word;
        int index = stoi(word);
        ss >> word;
        char data = word[0];
        Insert* obj = new Insert(index,data,name,clock);
        CQ.push((Command*)obj);
        clock++;
             cout << "command processed" << endl;
        continue;
     }
     if(word == "DELETE"){
        ss >> word;
        int index = stoi(word);
        Delete* obj = new Delete(index);
        CQ.push((Command*)obj);
             cout << "command processed" << endl;
        continue;
     }

    }
    // closing the socket.
    close(serverSocket);
    return NULL;
}


void* processor(void* arg){

        // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8082);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    int e_code = -1;
    while (e_code == -1){
    e_code =  connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
    }

    // sending data
  //  const char* message = "Hello, server!";
  //  send(clientSocket, message, strlen(message), 0);

    while(true){
        if(CQ.empty()){continue;}
        Command* lul = CQ.front();
        if(lul->this_type() == INSERT){
            
            Insert* curr = (Insert*) CQ.front();
            cout << curr->payload << " this is the payload at " << curr->index << endl; 
            state.insertNode(curr->index,curr->ts,curr->payload);
            string s_tree = state.serialize();
            cout << s_tree << "This is the serialized tree" << endl;
            uint32_t dataLength = htonl(s_tree.size());
            send(clientSocket,&dataLength ,sizeof(uint32_t) ,MSG_CONFIRM);
            send(clientSocket,s_tree.c_str(),s_tree.size(),MSG_CONFIRM);
             cout << "Inserted stuff updating remote" << endl;
        }
        else if(lul->this_type() == DELETE){
            
             Delete* curr = (Delete*) CQ.front();
             state.markDeleted(curr->index);
             string s_tree = state.serialize();
            uint32_t dataLength = htonl(s_tree.size());
            send(clientSocket,&dataLength ,sizeof(uint32_t) ,MSG_CONFIRM);
            send(clientSocket,s_tree.c_str(),s_tree.size(),MSG_CONFIRM);
            cout << "deleted stuff updating remote" << endl;
        }
        else if(lul->this_type() == MERGE){
              //cout << merging stuff << endl;
              Merge* curr = (Merge*) CQ.front(); 
              state.merge(curr->otherList);
              cout << state.getInorderTraversal() << endl;
        }
        else{
            cout << "Unknown command" << endl;
        }
        delete CQ.front();
        CQ.pop();
    }
    // closing socket
    close(clientSocket);
   return NULL;
}

std::vector<Node> deserialize(std::string sen){

    std::vector<Node> outp;
    std::stringstream ss(sen);
    std::string word;
    ss >> word;
    int size = stoi(word);
    for(int i = 0;i < size;i++){
        ss >> word;
        std::stringstream ns(word);
        std::string data;
        std::string tombstone;
        std::string id;
        std::string ts;
        getline(ns,data,':');
        getline(ns,tombstone,':');
        getline(ns,id,':');
        getline(ns,ts,':');
        char d = data[0];
        bool t = tombstone[0] == '1' ? true : false;
        long long time = std::stoll(ts);
        outp.push_back(Node(CausalDot(id,time),d,t));

    }
    return outp;
}


void*  peer_server(void* arg){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8081);
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

    while(true){

    uint32_t dataLength;
    int stat = recv(clientSocket,&dataLength,sizeof(uint32_t),0); // Receive the message length
    if(stat == 0){
        break;
    }
    dataLength = ntohl(dataLength); // Ensure host system byte order

    std::vector<char> rcvBuf;    // Allocate a receive buffer
    rcvBuf.resize(dataLength,0x00); // with the necessary size

    recv(clientSocket,&(rcvBuf[0]),dataLength,0); // Receive the string data

    std::string receivedString;                        // assign buffered data to a 
    receivedString.assign(&(rcvBuf[0]),rcvBuf.size()); // string
    
    vector<Node> new_tree  = deserialize(receivedString);
     Merge* obj = new Merge(new_tree);
    CQ.push((Command*)obj);

    }
    // closing the socket.
    close(serverSocket);
    return NULL;
}





int main()
{    // creating socket
    pthread_t front,back_client,back_server; 
    pthread_create(&front, NULL, &FrontConnect, NULL);
    pthread_create(&back_client, NULL, &processor, NULL);
    pthread_create(&back_server, NULL, &peer_server, NULL);
    pthread_join(front,NULL);
    pthread_join(back_client,NULL);
    pthread_join(back_server,NULL);
    cout << state.getInorderTraversal() << endl;
    return 0;
}