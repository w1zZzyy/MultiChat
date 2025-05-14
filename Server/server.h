#pragma once

#include <network_utils.h>

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>





class Server
{
public:

    Server(const std::string& address);
    ~Server();

    void call_stop();

private:

    int                         serverSocket;
    sockaddr_in                 serverAddr;

    std::list<int>              clientSockets;

    std::queue<std::pair
    <int, std::string>>         msgs;

    std::mutex                  clientMTX;
    std::mutex                  msgMTX;

    std::condition_variable     msgCV;

    std::thread                 acceptor;
    std::thread                 receiver;
    std::thread                 sender;
    std::atomic<bool>           stop;
};