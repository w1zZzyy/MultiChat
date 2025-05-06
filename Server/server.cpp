#include "server.h"

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <stdexcept>
#include <cstring>

Server::Server(const std::string& address, const std::string& display_address)
{
    stop.store(false);

    CreateAddress(address,          serverSocket,   serverAddr);
    CreateAddress(display_address,  displaySocket,  displayAddr);
    
    if(bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::runtime_error("server bind error\n");
    }

    if(listen(serverSocket, 10) == -1) {
        throw std::runtime_error("server listen error\n");
    }

    if(connect(displaySocket, (sockaddr*)&displayAddr, sizeof(displayAddr)) == -1) {
        throw std::runtime_error("server connect to display error\n");
    }


    acceptor = std::thread
    (
        [&]()
        {
            while(true)
            {
                socklen_t len = sizeof(serverAddr);
                int clientSocket = accept(serverSocket, (sockaddr*)&serverAddr, &len);

                if(stop.load()) {
                    break;
                }

                if(clientSocket == -1){
                    std::cerr << "clientSocket error\n";
                    continue;
                }

                std::lock_guard<std::mutex> lg(clientMTX);

                clientSockets.push_back(clientSocket);
            }
        }
    );


    receiver = std::thread
    (
        [&]()
        {
            char buffer[BUFF_SIZE];
                
            while(!stop.load())
            {
                std::lock_guard<std::mutex> lg(clientMTX);

                for(auto sock : clientSockets)
                {
                    memset(buffer, 0, BUFF_SIZE);
                    
                    int bytes = recv(sock, buffer, BUFF_SIZE, MSG_DONTWAIT);
                    
                    if(bytes == -1) {
                        continue;
                    }

                    if(bytes == 0) {
                        continue;
                    }

                    if(bytes < BUFF_SIZE) {
                        buffer[bytes] = '\0';
                    }

                    if(send(displaySocket, buffer, sizeof(buffer), 0) == -1) {
                        std::cerr << "server send error\n";
                    }
                }
            }
        }
    );
}

Server::~Server()
{
    close(serverSocket);

    stop.store(true);
    
    acceptor.join();
    receiver.join();
}

void Server::call_stop()
{
    stop.store(true);

    int stop_socket = socket(AF_INET, SOCK_STREAM, 0);
    connect(stop_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)); 
}
