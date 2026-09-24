#include "mailbox.hpp"
#include <glog/logging.h>

// Конструктор (Открываем БД)
Mailbox::Mailbox(const std::string& db_file) {
    if (sqlite3_open(db_file.c_str(), &db) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Failed to open database: " + err);
    }
    
    // Создаем таблицу
    createTable();
    LOG(INFO) << "Database initialized successfully: " << db_file;
}

// Деструктор (Закрываем БД)
Mailbox::~Mailbox() {
    if (db) {
        sqlite3_close(db);
    }
}

// Создание таблицы
void Mailbox::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS messages ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "sender TEXT, "
                      "subject TEXT, "
                      "date TEXT, "
                      "body TEXT);";
                      
    char* err_msg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::string err = err_msg;
        sqlite3_free(err_msg); // Обязательно очищаем память ошибки
        throw std::runtime_error("Failed to create table: " + err);
    }
}

// Сохранение письма (Используем безопасные Prepared Statements)
bool Mailbox::saveMessage(const Message& msg) {
    const char* sql = "INSERT INTO messages (sender, subject, date, body) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    // Подготавливаем запрос
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG(ERROR) << "Failed to prepare insert statement: " << sqlite3_errmsg(db);
        return false;
    }
    
    // Привязываем переменные к знакам вопроса (?)
    // SQLITE_TRANSIENT говорит SQLite сделать свою копию строк
    sqlite3_bind_text(stmt, 1, msg.sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, msg.subject.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, msg.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, msg.body.c_str(), -1, SQLITE_TRANSIENT);
    
    // Выполняем запрос
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        LOG(ERROR) << "Failed to execute insert: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }
    
    // Очищаем память
    sqlite3_finalize(stmt);
    LOG(INFO) << "Message from '" << msg.sender << "' saved to database.";
    return true;
}

// Чтение всех писем
std::vector<Message> Mailbox::getAllMessages() {
    std::vector<Message> messages;
    const char* sql = "SELECT id, sender, subject, date, body FROM messages ORDER BY id ASC;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG(ERROR) << "Failed to prepare select statement: " << sqlite3_errmsg(db);
        return messages;
    }
    
    // Читаем строки одну за другой
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Message msg;
        msg.id = sqlite3_column_int(stmt, 0);
        
        // SQLite возвращает unsigned char*, нужно кастовать в обычный char*
        const unsigned char* sender = sqlite3_column_text(stmt, 1);
        if (sender) msg.sender = reinterpret_cast<const char*>(sender);
        
        const unsigned char* subject = sqlite3_column_text(stmt, 2);
        if (subject) msg.subject = reinterpret_cast<const char*>(subject);
        
        const unsigned char* date = sqlite3_column_text(stmt, 3);
        if (date) msg.date = reinterpret_cast<const char*>(date);
        
        const unsigned char* body = sqlite3_column_text(stmt, 4);
        if (body) msg.body = reinterpret_cast<const char*>(body);
        
        messages.push_back(msg);
    }
    
    sqlite3_finalize(stmt);
    return messages; 
}