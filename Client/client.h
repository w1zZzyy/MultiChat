#pragma once

#include "network_utils.h"

class Client
{
public:

    Client(const std::string& server_address, const std::string& name);

    void send_message(const std::string& msg);

private:

    int         clientSocket;
    sockaddr_in serverAddr;

    std::string name;
};