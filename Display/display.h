#pragma once

#include "network_utils.h"

class Display
{
public:

    Display(std::string address);
    ~Display();

    void receive() const;

private:

    int         serverSocket;
    int         displaySocket;
    sockaddr_in displayAddr;

};