#include "client.h"

#include <stdexcept>

Client::Client(const std::string& server_address, const std::string& name) : name(name)
{
    CreateAddress(server_address, clientSocket, serverAddr);

    if(connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::runtime_error("client could not connect");
    }
}

void Client::send_message(const std::string &msg)
{
    auto buffer = name + ":\n" + msg;

    if(send(clientSocket, buffer.c_str(), buffer.size(), 0) == -1) {
        throw std::runtime_error("client could not send message");
    }
}
