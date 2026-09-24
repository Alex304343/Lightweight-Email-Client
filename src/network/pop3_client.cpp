#include "pop3_client.hpp"
#include <glog/logging.h>
#include <sstream>

Pop3Client::Pop3Client(const Config& cfg) : config(cfg) {}

bool Pop3Client::connectAndLogin() {
    if (!socket.connectTo(config.host, config.pop3_port)) {
        LOG(ERROR) << "Failed to connect to POP3 server at " << config.host << ":" << config.pop3_port;
        return false;
    }

    // Читаем приветствие сервера (в POP3 успех всегда начинается с +OK)
    std::string greeting = socket.readLine();
    if (greeting.length() < 3 || greeting.substr(0, 3) != "+OK") {
        LOG(ERROR) << "Unexpected POP3 greeting: " << greeting;
        socket.closeConnection();
        return false;
    }

    std::string response;
    
    // Команда USER
    if (!sendCommand("USER " + config.username, response)) {
        LOG(ERROR) << "POP3 USER command failed.";
        socket.closeConnection();
        return false;
    }

    // Команда PASS
    if (!sendCommand("PASS " + config.password, response)) {
        LOG(ERROR) << "POP3 PASS command failed (wrong password?).";
        socket.closeConnection();
        return false;
    }

    LOG(INFO) << "Successfully logged in to POP3 server.";
    return true;
}

int Pop3Client::getMessagesCount() {
    std::string response;
    // Команда STAT возвращает "+OK 3 1250" (3 письма, общий размер 1250 байт)
    if (!sendCommand("STAT", response)) {
        return -1;
    }

    std::istringstream iss(response);
    std::string ok_part;
    int count = 0;
    
    iss >> ok_part >> count; // Разбиваем строку по пробелам
    return count;
}

bool Pop3Client::getMessage(int index, Message& out_msg) {
    std::string response;
    // Отправляем RETR 1 (скачать письмо №1)
    if (!sendCommand("RETR " + std::to_string(index), response)) {
        LOG(ERROR) << "Failed to retrieve message " << index;
        return false;
    }

    // Если сервер ответил +OK, дальше он шлет текст письма, пока не встретит ".\r\n"
    std::string raw_email = "";
    while (true) {
        std::string line = socket.readLine();
        if (line.empty()) {
            LOG(ERROR) << "Connection dropped while downloading message.";
            return false;
        }

        if (line == ".\r\n") {
            break; // Конец письма
        }

        // Защита от точек (dot-stuffing): если строка начинается с "..", убираем одну
        if (line.length() >= 2 && line[0] == '.' && line[1] == '.') {
            line = line.substr(1);
        }

        raw_email += line;
    }

    // --- Простой парсинг письма ---
    // В письмах SMTP/POP3 заголовки отделяются от текста пустой строкой (\r\n\r\n)
    size_t split_pos = raw_email.find("\r\n\r\n");
    std::string headers = raw_email.substr(0, split_pos);
    std::string body = (split_pos != std::string::npos) ? raw_email.substr(split_pos + 4) : raw_email;

    out_msg.sender = extractHeader(headers, "From:");
    out_msg.subject = extractHeader(headers, "Subject:");
    out_msg.date = extractHeader(headers, "Date:");
    out_msg.body = body;

    LOG(INFO) << "Message " << index << " downloaded successfully.";
    return true;
}

void Pop3Client::disconnect() {
    if (socket.isConnected()) {
        std::string response;
        sendCommand("QUIT", response);
        socket.closeConnection();
    }
}

// Приватный метод для отправки команд POP3
bool Pop3Client::sendCommand(const std::string& cmd, std::string& out_response) {
    if (!socket.sendString(cmd + "\r\n")) {
        return false;
    }

    out_response = socket.readLine();
    if (out_response.length() < 3) return false;

    // В POP3 все успешные ответы начинаются с +OK, а ошибки с -ERR
    return out_response.substr(0, 3) == "+OK";
}

std::string Pop3Client::extractHeader(const std::string& headers, const std::string& header_name) {
    size_t pos = headers.find(header_name);
    if (pos == std::string::npos) return "Unknown";

    pos += header_name.length();
    
    // Пропускаем пробелы после двоеточия
    while (pos < headers.length() && (headers[pos] == ' ' || headers[pos] == '\t')) pos++;

    // Находим конец строки
    size_t end_pos = headers.find("\r\n", pos);
    if (end_pos == std::string::npos) end_pos = headers.length();

    return headers.substr(pos, end_pos - pos);
}