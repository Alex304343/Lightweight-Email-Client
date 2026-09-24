#pragma once

#include "tcp_socket.hpp"
#include "core/config.hpp"
#include <string>

class SmtpClient {
public:
    SmtpClient(const Config& config);

    bool sendEmail(const std::string& to, const std::string& subject, const std::string& body);


private:
    bool sendCommand(const std::string& cmd, const std::string& expected_code);
    TcpSocket socket; // По значению потому
    const Config& config;
};
