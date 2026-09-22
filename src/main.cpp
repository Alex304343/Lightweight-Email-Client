#include <glog/logging.h>
#include <iostream>
#include <exception>
#include "core/config.hpp"

int main(int argc, char* argv[]) {
    
    Config config;
    
    try{
        config = Config("config.yaml");
    }catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return 1;
    }


    return 0;
}