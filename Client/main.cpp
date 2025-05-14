#include "client.h"

#include <iostream>
#include <cassert>
#include <stdexcept>

int main(int argc, char* argv[]) 
{
    assert(argc == 2);

    std::cout << "Client run start\n";

    try
    {
        Client client(argv[1]);

        std::string message;
        while(std::getline(std::cin, message)){
            client.send_message(message);
        }
    }

    catch(std::runtime_error& err) {
        std::cerr << err.what();
    }

    catch(...) {
        std::cerr << "unexpected error\n";
    }

    std::cout << "Client run end\n";
}