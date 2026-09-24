#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>
#include <stdexcept>

// Структура для хранения одного письма
struct Message {
    int id = 0; // ID в базе данных
    std::string sender;
    std::string subject;
    std::string date;
    std::string body;
};

class Mailbox {
public:
    Mailbox(const std::string& db_file_name);
    
    // Деструктор закроет соединение с БД
    ~Mailbox();

    // Запрещаем копирование объекта (чтобы случайно не скопировать указатель на БД)
    Mailbox(const Mailbox&) = delete;
    Mailbox& operator=(const Mailbox&) = delete;

    bool saveMessage(const Message& msg);

    std::vector<Message> getAllMessages();

private:
    sqlite3* db = nullptr; // Указатель на базу данных SQLite

    // Приватный метод для создания таблицы, если её еще нет
    void createTable();
};