#pragma once

#include "core/config.hpp"
#include "sqlite/mailbox.hpp"

class ConsoleUI {
public:
    ConsoleUI(const Config& config, Mailbox& db);

    // Главный цикл программы
    void run();

private:
    // Обработчики пунктов меню
    void displayMenu();
    void checkInbox();
    void readMessage();
    void composeEmail();
    void viewLocalMessages();

    const Config& config;
    Mailbox& db;
};