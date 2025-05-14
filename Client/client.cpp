#include "client.h"

#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <limits>


Client::Client(const std::string& server_address)
{
    CreateAddress(server_address, clientSocket, serverAddr);

    if(connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::runtime_error("client could not connect\n");
    }

    while (true) 
    {
        std::cout << "Enter your name: ";
        std::getline(std::cin, name);
    
        if (name.length() == 0 || name.length() > NAME_LENGTH) {
            std::cout << "Name must be between 1 and 32 characters.\n";
            continue;
        }
    
        break;
    }

    send_message("", ClientConnected);

    receiver = std::thread
    (
        [&]
        {
            char buffer[BUFF_SIZE]; 
                         
            while(!stop.load())
            {
                std::memset(buffer, 0, BUFF_SIZE);

                int bytes = recv(clientSocket, buffer, BUFF_SIZE - 1, 0);

                if(bytes > 0) {
                    buffer[bytes] = '\0';
                    std::cout << buffer;
                }
                
                else if(bytes == 0) {
                    break;
                }

                else throw std::runtime_error("client recv error\n");
            }
        }
    );
}

Client::~Client()
{
    send_message("", ClientDisconnected);

    stop.store(true);
    shutdown(clientSocket, SHUT_RDWR);
    receiver.join();
    close(clientSocket);
}

void Client::send_message(const std::string &msg, MessageType msgFlag)
{
    size_t name_len = name.length(), msg_len = msg.length();

    char info[MSG_INFO_SIZE];
    char* data = info;


    std::memcpy(data, reinterpret_cast<char*>(&msgFlag), FLAG_SIZE);
    data += FLAG_SIZE;

    std::memcpy(data, reinterpret_cast<char*>(&name_len), ST_SIZE);
    data += ST_SIZE;

    std::memcpy(data, name.c_str(), name_len);
    data += name_len;

    std::memcpy(data, reinterpret_cast<char*>(&msg_len), ST_SIZE);
    data += ST_SIZE;


    if(send(clientSocket, info, MSG_INFO_SIZE, 0) == -1) {
        throw std::runtime_error("client could not send info\n");
    }

    if(msgFlag & ClientConnStatus) return;

  
    auto buffer = msg.c_str();
    size_t total_sent = 0;

    while (total_sent < msg_len) 
    {
        int sent = send(clientSocket, buffer + total_sent, msg_len - total_sent, 0);
        if (sent == -1) {
            throw std::runtime_error("client could not send message\n");
        }
        total_sent += sent;
    }
}

