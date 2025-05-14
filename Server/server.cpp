#include "server.h"

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <chrono>

Server::Server(const std::string& address)
{
    stop.store(false);

    CreateAddress(address, serverSocket, serverAddr);
    
    if(bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::runtime_error("server bind error\n");
    }

    if(listen(serverSocket, 10) == -1) {
        throw std::runtime_error("server listen error\n");
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
            char info[MSG_INFO_SIZE];
            char buffer[BUFF_SIZE];
                
            while(!stop.load())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::lock_guard<std::mutex> lg(clientMTX);

                for(auto client = clientSockets.begin(); client != clientSockets.end(); ++client)
                {
                    std::memset(buffer, 0, BUFF_SIZE);
                    std::memset(info, 0, MSG_INFO_SIZE);
                    
                    int info_bytes = recv(*client, info, MSG_INFO_SIZE, MSG_DONTWAIT);

                    if(info_bytes > 0)
                    {
                        if(info_bytes != MSG_INFO_SIZE) {
                            throw std::runtime_error("incorrect number of bytes in info\n");
                        }

                        MessageType msg_flag;
                        size_t name_len, msg_len;
                        std::string name, msg;

                        auto data = info;

                        
                        std::memcpy(reinterpret_cast<char*>(&msg_flag), data, FLAG_SIZE);
                        data += FLAG_SIZE;

                        std::memcpy(reinterpret_cast<char*>(&name_len), data, ST_SIZE);
                        data += ST_SIZE;

                        name.resize(name_len);

                        std::memcpy(name.data(), data, name_len);
                        data += name_len;


                        if(msg_flag & ClientConnStatus) 
                        {
                            msg = ((msg_flag == ClientConnected) 
                                        ? "--Connected--" 
                                        : "--Disconnected--");
                            name += ' ';
                        }

                        else if(msg_flag == TextMSG)
                        {
                            std::memcpy(reinterpret_cast<char*>(&msg_len), data, ST_SIZE);
                            data = buffer;

                            for(size_t msg_bytes = 0; msg_bytes < msg_len;)
                            {
                                int buffer_bytes = recv(*client, buffer + msg_bytes, msg_len - msg_bytes, 0);
                                
                                if(buffer_bytes <= 0) {
                                    throw std::runtime_error("Not all data was received\n");
                                }

                                msg_bytes += buffer_bytes;
                            }

                            msg = std::string(buffer, msg_len);
                            name += ":\n";
                        }

                        else throw std::runtime_error("server flag error\n");


                        {
                            std::lock_guard<std::mutex> lg(msgMTX);
                            msgs.push({*client, name + msg + "\n"});
                        }
                        msgCV.notify_one();
                    }

                }
            }
        }
    );

    sender = std::thread
    (
        [&]
        {
            while(!stop.load())
            {
                std::unique_lock<std::mutex> ul(msgMTX);
                msgCV.wait(ul, [&]{return !msgs.empty() || stop;});

                if(msgs.empty()) break;

                auto [client_sender, msg] = msgs.front();
                msgs.pop();

                ul.unlock();


                auto now = std::chrono::system_clock::now();               
                std::time_t now_c = std::chrono::system_clock::to_time_t(now); 
                std::string timeStr = std::ctime(&now_c);
                timeStr.pop_back(); 

                auto infoText = "[" + timeStr + "] " + msg;


                std::lock_guard<std::mutex> lg(clientMTX);
                for(auto& client : clientSockets) {
                    if(client != client_sender) {
                        if(send(client, infoText.c_str(), infoText.size(), 0) == -1) {
                            throw std::runtime_error("server to client send error\n");
                        }
                    }
                }
            }
        }
    );
}

Server::~Server()
{
    call_stop();
}

void Server::call_stop()
{
    stop.store(true);
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);

    acceptor.join();
    receiver.join();
    sender.join();

    std::lock_guard<std::mutex> lg(clientMTX);
    for (int client : clientSockets) {
        shutdown(client, SHUT_RDWR);
        close(client);
    }
}
