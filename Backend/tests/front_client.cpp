#include <pthread.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <queue>


void* processor(void* arg){
int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8083);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    int e_code = -1;
    while (e_code == -1){
    e_code =  connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
    }

    // sending data
    const char* message1 = "INSERT 0 a";
    const char* message2 = "INSERT 1 b";
    send(clientSocket, message1, strlen(message1), 0);
    send(clientSocket, message2, strlen(message2), 0);
    //send(clientSocket, message, strlen(message), 0);
    // while(true){
    
    // }
    close(clientSocket);
    return NULL;
}

int main(){
    pthread_t pid;
    pthread_create(&pid,NULL,&processor,NULL);
    pthread_join(pid,NULL);
    return 0;
}