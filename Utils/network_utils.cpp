#include "network_utils.h"

#include <arpa/inet.h>

#include <stdexcept>
#include <sstream>

void CreateAddress(const std::string& address, int& sck, sockaddr_in &addr)
{
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == -1) {
        throw std::runtime_error("socket error");
    }

    std::stringstream ss(address);

    std::string ip, port;

    std::getline(ss, ip, ':');
    std::getline(ss, port);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(std::stoi(port));

    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        throw std::runtime_error("ip error");
    }
}