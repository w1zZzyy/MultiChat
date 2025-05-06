#include "client.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

int main(int argc, char* argv[]) 
{
    assert(argc == 3);

    std::cout << "Client run start\n";

    try
    {
        Client client(argv[1], argv[2]);

        std::string message;
        while(std::cin >> message){
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

    std::cin.get();
}