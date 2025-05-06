#include "display.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

int main(int argc, char* argv[]) 
{
    assert(argc == 2);

    std::cout << "Display run start\n";

    try
    {
        Display display(argv[1]);

        while(true){
            display.receive();
        }
    }

    catch(std::runtime_error& err) {
        std::cerr << err.what();
    }

    catch(...) {
        std::cerr << "unexpected error\n";
    }

    std::cout << "Display run end\n";
}