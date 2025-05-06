#include "server.h"
#include <iostream>
#include <cassert>

int main(int argc, char* argv[]) 
{
    assert(argc == 3);

    std::cout << "Server run start\n";

    try
    {
        Server server(argv[1], argv[2]);

        while(true)
        {
            std::string command;
            std::cin >> command;

            if(command == "stop")
            {
                server.call_stop();
                break;
            }
        }
    }

    catch(std::runtime_error& err) {
        std::cerr << err.what();
    }

    catch(...) {
        std::cerr << "unexpected error\n";
    }

    std::cout << "Server run end\n";

    std::cin.get();
}