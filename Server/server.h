#pragma once

#include <network_utils.h>

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>


class Server
{
public:

    Server(const std::string& address, const std::string& display_address);
    ~Server();

    void call_stop();

private:

    int                         serverSocket;
    sockaddr_in                 serverAddr;

    int                         displaySocket;
    sockaddr_in                 displayAddr;

    std::list<int>              clientSockets;

    std::mutex                  clientMTX;
    std::thread                 acceptor;
    std::thread                 receiver;
    std::atomic<bool>           stop;
};