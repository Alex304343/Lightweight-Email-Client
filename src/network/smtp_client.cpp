#include "smtp_client.hpp"

#include <glog/logging.h>
SmtpClient::SmtpClient(const Config& config) : config(config) {}
bool SmtpClient::sendEmail(const std::string& to, const std::string& subject, const std::string& body){
    // Подключаемся
    if (!socket.connectTo(config.host, config.smtp_port)){
        LOG(ERROR) << "Failed to connect to SMTP server at " << config.host << ":" << config.smtp_port;
        return false;
    } 

    // Читаем приветствие сервера (код 220)
    std::string greeting = socket.readLine();
    if (greeting.length() < 3 || greeting.substr(0, 3) != "220") {
        socket.closeConnection();
        LOG(ERROR) << "Unexpected SMTP greeting: " << greeting;
        return false;
    }

    // 2. Выполняем диалог SMTP (используя наш приватный метод)
    if (!sendCommand("HELO " + config.host, "250")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send HELO command.";
        return false;
    } 
    if (!sendCommand("MAIL FROM:<" + config.username + ">", "250")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send MAIL FROM command.";
        return false;
    }
    if (!sendCommand("RCPT TO:<" + to + ">", "250")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send RCPT TO command.";
        return false;
    }
    
    LOG(INFO) << "Sending email to: " << to << " with subject: " << subject;
    
    // Команда DATA ждет код 354
    if (!sendCommand("DATA", "354")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send DATA command.";
        return false;
    } 

    // Отправляем само письмо...
    if(!socket.sendString("Subject: " + subject + "\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email subject.";
        return false;
    }


    if(!socket.sendString("From: " + config.username + "\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email From header.";
        return false;
    }
    
    if(socket.sendString("To: " + to + "\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email To header.";
        return false;
    }

    // Пустая строка отделяет заголовки от тела
    if(!socket.sendString("\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email header-body separator.";
        return false;
    }
    if(!socket.sendString(body + "\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email body.";
        return false;
    }
    // Одиночная точка означает конец письма
    if(!socket.sendString(".\r\n")){
        socket.closeConnection();
        LOG(ERROR) << "Failed to send email termination.";
        return false;
    } 

    std::string data_resp = socket.readLine(); 
    if (data_resp.length() < 3 || data_resp.substr(0, 3) != "250") {
        LOG(ERROR) << "Failed to send email data: " << data_resp;
        socket.closeConnection();
        return false;
    }


    // 3. Прощаемся и кладем трубку
    sendCommand("QUIT", "221");
    socket.closeConnection();

    LOG(INFO) << "Email sent successfully to: " << to;

    return true;
}
bool SmtpClient::sendCommand(const std::string& cmd, const std::string& expected_code) {
    socket.sendString(cmd + "\r\n");
    std::string response = socket.readLine();
    
    if (response.length() < 3) {
        return false;
    }
    
    // Проверяем, начинается ли ответ с нужного кода
    return response.substr(0, 3) == expected_code;
}