#include "console_ui.hpp"
#include "network/smtp_client.hpp"
#include "network/pop3_client.hpp"

#include <iostream>
#include <string>
#include <limits>

ConsoleUI::ConsoleUI(const Config& cfg, Mailbox& database) 
    : config(cfg), db(database) {}

void ConsoleUI::run() {
    int choice = 0;
    while (choice != 5) {
        displayMenu();
        std::cout << "Select an option: ";
        
        // Безопасное чтение числа
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        // Очищаем буфер после ввода числа, чтобы getline работал корректно
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << std::string(40, '-') << "\n"; // Разделитель

        switch (choice) {
            case 1: checkInbox(); break;
            case 2: readMessage(); break;
            case 3: composeEmail(); break;
            case 4: viewLocalMessages(); break;
            case 5: std::cout << "Exiting... Goodbye!\n"; break;
            default: std::cout << "Invalid option. Try again.\n"; break;
        }
    }
}

void ConsoleUI::displayMenu() {
    std::cout << "\n========================================\n";
    std::cout << "               EMAIL CLIENT               \n";
    std::cout << "========================================\n";
    std::cout << "1. Check Inbox (Download via POP3)\n";
    std::cout << "2. Read Message\n";
    std::cout << "3. Compose and Send Email (SMTP)\n";
    std::cout << "4. View Local Messages\n";
    std::cout << "5. Exit\n";
}

void ConsoleUI::checkInbox() {
    std::cout << "[*] Connecting to POP3 server...\n";
    
    Pop3Client pop3(config);
    if (!pop3.connectAndLogin()) {
        std::cout << "[-] Failed to login to POP3 server.\n";
        return;
    }

    int count = pop3.getMessagesCount();
    if (count < 0) {
        std::cout << "[-] Failed to get messages count.\n";
        pop3.disconnect();
        return;
    }

    std::cout << "[+] Found " << count << " messages on the server.\n";

    int downloaded = 0;
    for (int i = 1; i <= count; ++i) {
        Message msg;
        if (pop3.getMessage(i, msg)) {
            if (db.saveMessage(msg)) {
                downloaded++;
                pop3.deleteMessage(i);
            }
        }
    }

    pop3.disconnect();
    std::cout << "[+] Successfully downloaded and saved " << downloaded << " messages.\n";
}

void ConsoleUI::readMessage() {
    viewLocalMessages(); // Сначала показываем список
    
    std::cout << "\nEnter the ID of the message to read (0 to cancel): ";
    int id;
    if (!(std::cin >> id) || id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    // Ищем письмо
    auto messages = db.getAllMessages();
    bool found = false;
    for (const auto& msg : messages) {
        if (msg.id == id) {
            std::cout << "\n========================================\n";
            std::cout << "From:    " << msg.sender << "\n";
            std::cout << "Date:    " << msg.date << "\n";
            std::cout << "Subject: " << msg.subject << "\n";
            std::cout << "----------------------------------------\n";
            std::cout << msg.body << "\n";
            std::cout << "========================================\n";
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "[-] Message with ID " << id << " not found.\n";
    }
}

void ConsoleUI::composeEmail() {
    std::string to, subject, body, line;

    std::cout << "To: ";
    std::getline(std::cin, to);

    std::cout << "Subject: ";
    std::getline(std::cin, subject);

    std::cout << "Message Body (Type '.' on a new line to finish and send):\n";
    std::cout << "---------------------------------------------------------\n";
    
    // Читаем многострочный ввод текста
    while (true) {
        std::getline(std::cin, line);
        if (line == ".") {
            break; // Конец ввода
        }
        body += line + "\n";
    }

    std::cout << "[*] Sending email...\n";
    
    SmtpClient smtp(config);
    if (smtp.sendEmail(to, subject, body)) {
        std::cout << "[+] Email sent successfully!\n";
    } else {
        std::cout << "[-] Failed to send email.\n";
    }
}

void ConsoleUI::viewLocalMessages() {
    auto messages = db.getAllMessages();
    if (messages.empty()) {
        std::cout << "[!] Mailbox is empty.\n";
        return;
    }

    std::cout << "\n--- LOCAL MAILBOX ---\n";
    for (const auto& msg : messages) {
        std::cout << "[" << msg.id << "] " 
                  << "From: " << msg.sender << " | "
                  << "Subject: " << msg.subject << "\n";
    }
}