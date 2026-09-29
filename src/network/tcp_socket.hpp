#pragma once

#include <string>

class TcpSocket {

public:
    TcpSocket();
    ~TcpSocket();

    // Подключение к серверу (например, host="127.0.0.1" или "localhost", port=25)
    bool connectTo(const std::string& host, int port);

    // Отправка строки (команды)
    bool sendString(const std::string& data);

    // Чтение ответа от сервера до символов \r\n
    std::string readLine();

    // Закрытие соединения
    void closeConnection();

    // Проверка, подключен ли сокет
    bool isConnected() const;

private:
    int sock_fd; // Дескриптор сокета (-1, если не подключен)

};