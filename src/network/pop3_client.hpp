#pragma once

#include "network/tcp_socket.hpp"
#include "core/config.hpp"
#include "sqlite/mailbox.hpp" // Только ради структуры Message
#include <string>

class Pop3Client {
public:
    Pop3Client(const Config& config);

    // Подключиться и авторизоваться
    bool connectAndLogin();

    // Получить количество писем
    // Возвращает -1 в случае ошибки
    int getMessagesCount();

    bool getMessage(int index, Message& out_msg);

    void disconnect();

private:
    // Отправка команды и получение однострочного ответа
    bool sendCommand(const std::string& cmd, std::string& out_response);
    
    // Парсер для извлечения Subject, From, Date из текста письма
    std::string extractHeader(const std::string& headers, const std::string& header_name);

    TcpSocket socket;
    const Config& config;
};