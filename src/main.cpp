#include <glog/logging.h>
#include <iostream>
#include <exception>

#include "logger/logger.hpp"
#include "core/config.hpp"

int main(int argc, char* argv[]) {
    
    Config config;
    
    try{
        config = Config("config.yaml");
    }catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Configuration loaded successfully." << std::endl;

    initLogger(argv[0], config);

    LOG(INFO) << "Logger initialized successfully.";

    return 0;
}