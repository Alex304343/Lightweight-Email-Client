#include "tcp_socket.hpp"
#include <glog/logging.h>

// Системные библиотеки Linux для работы с сетью
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring> // для memset

TcpSocket::TcpSocket() : sock_fd(-1) {}

TcpSocket::~TcpSocket() {
    closeConnection();
}

bool TcpSocket::connectTo(const std::string& host, int port) {
    if (isConnected()) {
        LOG(WARNING) << "Socket is already connected. Closing old connection.";
        closeConnection();
    }

    // 1. Создаем сокет (IPv4, TCP)
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        LOG(ERROR) << "Failed to create socket.";
        return false;
    }

    // 2. Резолвим IP-адрес (переводим домен в ip)
    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        LOG(ERROR) << "No such host: " << host;
        closeConnection();
        return false;
    }

    // 3. Настраиваем структуру с адресом
    struct sockaddr_in serv_addr; // Информация о сервере, к которому мы подключаемся
    memset(&serv_addr, 0, sizeof(serv_addr)); // Обнуляем структуру
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    // Копируем IP-адрес из структуры hostent в нашу serv_addr
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    // 4. Подключаемся
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        LOG(ERROR) << "Connection failed to " << host << ":" << port;
        closeConnection();
        return false;
    }

    LOG(INFO) << "Successfully connected to " << host << ":" << port;
    return true;
}

bool TcpSocket::sendString(const std::string& data) {
    if (!isConnected()) {
        LOG(ERROR) << "Cannot send data: socket is not connected.";
        return false;
    }

    // Отправляем данные. Важно: send может отправить не все данные за один вызов

    size_t total_sent = 0;
    size_t bytes_left = data.length();

    while (total_sent < data.length()) {
        ssize_t sent_bytes = send(sock_fd, data.c_str() + total_sent, bytes_left, 0);
        
        if (sent_bytes < 0) {
            LOG(ERROR) << "Failed to send data.";
            return false;
        }

        total_sent += sent_bytes;
        bytes_left -= sent_bytes;
    }

    std::string log_data = data;
    size_t pos = 0;
    while ((pos = log_data.find("\r", pos)) != std::string::npos) log_data.replace(pos, 1, "\\r"), pos += 2;
    pos = 0;
    while ((pos = log_data.find("\n", pos)) != std::string::npos) log_data.replace(pos, 1, "\\n"), pos += 2;

    LOG(INFO) << "Client send: " << log_data;
    return true;
}

std::string TcpSocket::readLine() {
    if (!isConnected()) {
        LOG(ERROR) << "Cannot read data: socket is not connected.";
        return "";
    }

    std::string result = "";
    char c;
    // Читаем по 1 байту
    while (recv(sock_fd, &c, 1, 0) > 0) {
        result += c;
        // Если последние два символа это \r\n, значит строка закончилась
        if (result.length() >= 2 && result.substr(result.length() - 2) == "\r\n") {
            break; 
        }
    }

    // Печатаем ответ сервера в лог (S: означает Server)
    if (!result.empty()) {
        // Делаем копию без \r\n для красивого вывода в лог
        std::string log_str = result;
        if (log_str.length() >= 2) {
            log_str.erase(log_str.length() - 2); 
        }
        LOG(INFO) << "S: " << log_str;
    }

    return result;
}

void TcpSocket::closeConnection() {
    if (sock_fd != -1) {
        close(sock_fd);
        LOG(INFO) << "Connection closed.";
        sock_fd = -1;
    }
}

bool TcpSocket::isConnected() const {
    return sock_fd != -1;
}