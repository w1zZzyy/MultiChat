cmake_minimum_required(VERSION 3.20)

project(MultiChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(Utils)

add_library(UtilsLib STATIC 
    Utils/network_utils.h Utils/network_utils.cpp
)

set(SERVER 
    Server/main.cpp 
    Server/server.cpp
    Server/server.h
)
 
set(CLIENT
    Client/main.cpp 
    Client/client.cpp
    Client/client.h
)

add_executable(ServerExe ${SERVER})
add_executable(ClientExe ${CLIENT})

target_link_libraries(ServerExe PRIVATE UtilsLib)
target_link_libraries(ClientExe PRIVATE UtilsLib)

add_compile_options(
    -Wall -Wextra -g -std=c++20
)
