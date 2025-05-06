#include "display.h"

#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <ctime>

Display::Display(const std::string& address)
{
    CreateAddress(address, displaySocket, displayAddr);

    if(bind(displaySocket, (sockaddr*)&displayAddr, sizeof(displayAddr)) == -1) {
        throw std::runtime_error("display bind error\n");
    }

    if(listen(displaySocket, 1) == -1) {
        throw std::runtime_error("display listen error");
    }

    socklen_t len = sizeof(displayAddr);

    serverSocket = accept(displaySocket, (sockaddr*)&displayAddr, &len);
    if(serverSocket == -1) {
        throw std::runtime_error("display acception error");
    }
}

Display::~Display()
{
    close(displaySocket);
}


void Display::receive() const
{
    static char buffer[BUFF_SIZE];
    memset(buffer, 0, BUFF_SIZE);

    int bytes = recv(serverSocket, buffer, BUFF_SIZE, 0);
    if(bytes == -1) {
        throw std::runtime_error("display receive error");
    }

    if(bytes > 0 && bytes < BUFF_SIZE) {
        buffer[bytes] = '\0';
    }

    std::string message(buffer, bytes); 

    auto now = std::chrono::system_clock::now();               
    std::time_t now_c = std::chrono::system_clock::to_time_t(now); 
    std::string timeStr = std::ctime(&now_c);
    timeStr.pop_back(); 

    std::cout << "[" << timeStr << "] " << message << '\n';
}
