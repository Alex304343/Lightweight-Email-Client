#include "config.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>

Config::Config(const std::string& filename){
    YAML::Node config = YAML::LoadFile(filename);

    // Сервер
    host = config["server"]["host"].as<std::string>();
    smtp_port = config["server"]["smtp_port"].as<int>();
    pop3_port = config["server"]["pop3_port"].as<int>();

    // Аккаунт
    username = config["account"]["username"].as<std::string>();
    password = config["account"]["password"].as<std::string>();

    // Хранилище
    db_file = config["storage"]["db_file"].as<std::string>();

    // Логирование
    log_to_stderr = config["logging"]["log_to_stderr"].as<bool>();
    color_log = config["logging"]["color_log"].as<bool>();
    min_log_level = config["logging"]["min_log_level"].as<int>();
}