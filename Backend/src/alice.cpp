// C++ program to show the example of server application in
// socket programming
#include <pthread.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <queue>
#include "../headers/ConcurrentLL.hpp"
#include "../headers/commands.hpp"
// #include "lclock.hpp"
using namespace std;
#include <thread>
#include <chrono>

queue<Command *> CQ;
ConcurrentLinkedList state;

std::string parseRequest(const std::string &request, std::string &method, std::string &route, std::string &body)
{
    stringstream ss(request);
    string line;
    getline(ss, line);

    // Parse method and route
    stringstream firstLine(line);
    firstLine >> method;
    firstLine >> route;

    // Parse the body
    size_t contentStart = request.find("\r\n\r\n");
    if (contentStart != string::npos)
    {
        body = request.substr(contentStart + 4);
    }

    return line;
}

void sendResponse(int clientSocket, const string &status, const string &body)
{
    string response = "HTTP/1.1 " + status + "\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Access-Control-Allow-Origin: *\r\n"; // Allow all origins
    response += "Content-Length: " + to_string(body.size()) + "\r\n\r\n";
    response += body;

    send(clientSocket, response.c_str(), response.size(), 0);
}

void *FrontConnect(void *arg)
{
    string name = "alice";
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        cerr << "Failed to create socket" << endl;
        return NULL;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        cerr << "Failed to bind socket" << endl;
        return NULL;
    }

    listen(serverSocket, 10);
    cout << "Server listening on port 8080" << endl;

    while (true)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0)
            continue;

        char buffer[4096] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            close(clientSocket);
            continue;
        }

        string request(buffer);
        // cout << "Received request: " << request << endl; // Print the received request
        string method, route, body;
        parseRequest(request, method, route, body);

        cout << "Method: " << method << ", Route: " << route << ", Body: " << body << endl; // Print parsed details

        if (method == "POST")
        {
            if (route == "/insert")
            {
                // Parse JSON payload for INSERT
                stringstream ss(body);
                int key;
                char data;
                int index;

                ss.ignore(256, ':');    // Skip to "char"
                //ss >> std::quoted(key); // Extract the quoted character as a string
                ss >> key;
                data = char(key);
                ss.ignore(256, ':'); // Skip to "index"
                ss >> index;

                std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
                long long timestamp = ns.count();

                cout << "Inserting " << data << " at index " << index << " with timestamp " << timestamp << endl;

                Insert *cmd = new Insert(index, data, name, timestamp);
                CQ.push((Command *)cmd);

                cout << "Command pushed to queue, length: " << CQ.size() << endl;

                sendResponse(clientSocket, "200 OK", R"({"status":"success"})");
            }
            else if (route == "/delete")
            {
                // Parse JSON payload for DELETE
                stringstream ss(body);
                int index;

                ss.ignore(256, ':'); // Skip to "index"
                ss >> index;

                Delete *cmd = new Delete(index);
                CQ.push(cmd);

                sendResponse(clientSocket, "200 OK", R"({"status":"success"})");
            }
        }
        else if (method == "GET" && route == "/text")
        {
            // Get the current state and return it
            string currentText = state.getInorderTraversal();
            sendResponse(clientSocket, "200 OK", R"({"text":")" + currentText + R"("})");
        }
        else if (method == "OPTIONS")
        {
            // Handle preflight request
            string response = "HTTP/1.1 204 No Content\r\n";
            response += "Access-Control-Allow-Origin: *\r\n"; // Allow all origins
            response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
            response += "Access-Control-Allow-Headers: Content-Type\r\n";
            response += "Content-Length: 0\r\n\r\n";

            send(clientSocket, response.c_str(), response.size(), 0);
        }
        else
        {
            sendResponse(clientSocket, "400 Bad Request", R"({"error":"Invalid route or method"})");
        }

        close(clientSocket);
    }

    close(serverSocket);
    return NULL;
}

void *processor(void *arg)
{

    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8092);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    std::cout << "Connecting to peer" << std::endl;
    int e_code = -1;
    while (e_code == -1)
    {
        e_code = connect(clientSocket, (struct sockaddr *)&serverAddress,
                         sizeof(serverAddress));
    }

    cout << "Connected to server" << endl;

    // sending data
    //  const char* message = "Hello, server!";
    //  send(clientSocket, message, strlen(message), 0);

    while (true)
    {
        // std::cout<<"Queue size: "<<CQ.size()<<std::endl;
        if (CQ.empty())
        {
            // cout << "command queue is empty" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        Command *lul = CQ.front();
        if (lul->this_type() == INSERT)
        {
            Insert *curr = (Insert *)CQ.front();
            cout << curr->payload << " this is the payload at " << curr->index << endl;
            state.insertNode(curr->index, curr->ts, curr->payload);
            string s_tree = state.serialize();
            cout << s_tree << "This is the serialized tree" << endl;
            uint32_t dataLength = htonl(s_tree.size());
            send(clientSocket, &dataLength, sizeof(uint32_t), MSG_CONFIRM);
            send(clientSocket, s_tree.c_str(), s_tree.size(), MSG_CONFIRM);
            cout << "Inserted stuff updating remote" << endl;
        }
        else if (lul->this_type() == DELETE)
        {

            Delete *curr = (Delete *)CQ.front();
            // print the tree before deleting
            std::cout << "pre delete " << state.getInorderTraversal() << std::endl;
            //
            std::cout << "current index " << curr->index << std::endl;
            state.markDeleted(curr->index);
            std::cout << "post delete " << state.getInorderTraversal() << std::endl;

            string s_tree = state.serialize();
            uint32_t dataLength = htonl(s_tree.size());
            send(clientSocket, &dataLength, sizeof(uint32_t), MSG_CONFIRM);
            send(clientSocket, s_tree.c_str(), s_tree.size(), MSG_CONFIRM);
            cout << "deleted stuff updating remote" << endl;
        }
        else if (lul->this_type() == MERGE)
        {
            cout << "merging stuff" << endl;
            Merge *curr = (Merge *)CQ.front();
            state.merge(curr->otherList);
            cout << state.getInorderTraversal() << endl;
        }
        else
        {
            cout << "Unknown command" << endl;
        }
        delete CQ.front();
        CQ.pop();
    }
    // closing socket
    close(clientSocket);
    return NULL;
}

std::vector<Node> deserialize(std::string sen)
{

    std::vector<Node> outp;
    std::stringstream ss(sen);
    std::string word;
    ss >> word;
    int size = stoi(word);
    for (int i = 0; i < size; i++)
    {
        ss >> word;
        std::stringstream ns(word);
        std::string data;
        std::string tombstone;
        std::string id;
        std::string ts;
        getline(ns, data, ':');
        getline(ns, tombstone, ':');
        getline(ns, id, ':');
        getline(ns, ts, ':');
        char d = char(stoi(data));
        bool t = tombstone[0] == '1' ? true : false;
        long long time = std::stoll(ts);
        outp.push_back(Node(CausalDot(id, time), d, t));
    }
    return outp;
}

void *peer_server(void *arg)
{

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8091);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr *)&serverAddress,
         sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, 1);

    // accepting connection request
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    // recieving data

    while (true)
    {

        uint32_t dataLength;
        int stat = recv(clientSocket, &dataLength, sizeof(uint32_t), 0); // Receive the message length
        if (stat == 0)
        {
            break;
        }
        dataLength = ntohl(dataLength); // Ensure host system byte order

        std::vector<char> rcvBuf;        // Allocate a receive buffer
        rcvBuf.resize(dataLength, 0x00); // with the necessary size

        recv(clientSocket, &(rcvBuf[0]), dataLength, 0); // Receive the string data

        std::string receivedString;                         // assign buffered data to a
        receivedString.assign(&(rcvBuf[0]), rcvBuf.size()); // string

        vector<Node> new_tree = deserialize(receivedString);
        Merge *obj = new Merge(new_tree);
        CQ.push((Command *)obj);
    }
    // closing the socket.
    close(serverSocket);
    return NULL;
}

int main()
{ // creating socket
    pthread_t front, back_client, back_server;
    pthread_create(&front, NULL, &FrontConnect, NULL);
    pthread_create(&back_client, NULL, &processor, NULL);
    pthread_create(&back_server, NULL, &peer_server, NULL);
    pthread_join(front, NULL);
    pthread_join(back_client, NULL);
    pthread_join(back_server, NULL);
    cout << state.getInorderTraversal() << endl;
    return 0;
}