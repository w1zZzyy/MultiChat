#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <cstdint>


using u8  = uint8_t;


constexpr size_t BUFF_SIZE      =   1024;
constexpr size_t FLAG_SIZE      =   sizeof(u8);
constexpr size_t ST_SIZE        =   sizeof(size_t);
constexpr size_t NAME_LENGTH    =   32;
constexpr size_t MSG_INFO_SIZE  =   FLAG_SIZE + NAME_LENGTH + 2 * ST_SIZE;


enum MessageType : u8
{
    ClientConnected     = 0b00000001,
    ClientDisconnected  = 0b00000010,
    ClientConnStatus    = 0b00000011, 
    TextMSG             = 0b00000100
};


void CreateAddress(const std::string& address, int& sck, sockaddr_in& addr);