#include <glog/logging.h>
#include <iostream>
#include <exception>

#include "logger/logger.hpp"
#include "core/config.hpp"
#include "sqlite/mailbox.hpp"
#include "ui/console_ui.hpp"

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

    try {
        Mailbox db(config.db_file);
        ConsoleUI ui(config, db);
        ui.run();
    } catch (const std::exception& e) {
        LOG(ERROR) << "Exception: " << e.what();
        return 1;
    }
    return 0;
}