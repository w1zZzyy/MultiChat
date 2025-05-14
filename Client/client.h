#pragma once

#include "network_utils.h"
#include <thread>
#include <atomic>


class Client
{
public:

    Client(const std::string& server_address);
    ~Client();


    //msg that going to be sent to server:
    /*
        1              byte     -   msg flag
        sizeof(size_t) bytes    -   name_len
        name_len       bytes    -   name
        sizeof(size_t) bytes    -   msg_len
        msg_len        bytes    -   msg
    */
    void send_message(const std::string& msg, MessageType msgFlag = TextMSG);

private:

    int         clientSocket;
    sockaddr_in serverAddr;

    std::thread receiver;
    std::atomic<bool> stop;

    std::string name;
};