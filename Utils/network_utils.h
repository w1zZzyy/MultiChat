#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <cstdint>


constexpr int BUFF_SIZE = 1024;


void CreateAddress(const std::string& address, int& sck, sockaddr_in& addr);