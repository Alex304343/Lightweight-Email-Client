#pragma once
#include <string>

class Config {
    public:
    // Сервер
    std::string host;
    int smtp_port;
    int pop3_port;

    // Аккаунт
    std::string username;
    std::string password;

    // Хранилище
    std::string db_file;

    // Логирование (настройки glog)
    bool log_to_stderr;
    bool color_log;
    int min_log_level;

    // Метод загрузки
    Config() = default;
    Config(const std::string& filename);
};