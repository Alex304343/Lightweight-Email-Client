# Lightweight Email Client (C++)

A lightweight, console-based email client written in C++ (C++23) for Linux. 
This project implements the **SMTP** and **POP3** application-layer protocols from scratch using raw POSIX TCP sockets, without relying on high-level networking libraries. 

It features a modular architecture, safe local storage using SQLite, and robust error handling and logging.

## 🌟 Features

* **SMTP Client:** Send plain-text emails (supports `HELO`, `MAIL FROM`, `RCPT TO`, `DATA`, `QUIT`).
* **POP3 Client:** Retrieve emails and safely delete them from the server after successful download (supports `USER`, `PASS`, `STAT`, `RETR`, `DELE`, `QUIT`).
* **Local Storage:** Downloaded messages are persistently stored in a local SQLite database using Prepared Statements.
* **Robust Logging:** Detailed, color-coded console logging using Google Glog. Passwords are intentionally masked in the logs for security.
* **Defensive Networking:** Handles TCP fragmentation, dropped connections, and ignores `SIGPIPE` signals (`MSG_NOSIGNAL`) to prevent application crashes.

## 🛠 Prerequisites (Linux)

To compile and run this project, you need `g++`, `make`, and the following libraries:

```bash
sudo apt update
sudo apt install libsqlite3-dev libyaml-cpp-dev libgoogle-glog-dev
```

## 🚀 Environment Setup (Test Mail Server)

For local testing, we use **GreenMail** via Docker. It provides an in-memory SMTP and POP3 server.
Run the following command to deploy the server and automatically create two test accounts (`acc1@localhost` and `acc2@localhost`):

```bash
docker run -d \
  -p 3025:3025 \
  -p 3110:3110 \
  -e GREENMAIL_OPTS="-Dgreenmail.setup.test.all -Dgreenmail.hostname=0.0.0.0 -Dgreenmail.users=acc1@localhost:123,acc2@localhost:321" \
  --name test-mail \
  greenmail/standalone:latest
```

## ⚙️ Configuration

`config.yaml` in the root directory of the project. This file configures the server addresses, ports, account details, and logging preferences.


## 🏗 Compilation and usage

The project uses a custom `Makefile` for efficient compilation with dependency tracking.

To build the project, simply run:
```bash
make
```
*(This will generate object files in the `build/` directory and create the `build/email_client` executable).*

To build and run the project:
```bash
make run
```
*(This will run `make` and then run program)

To clean build artifacts:
```bash
make clean
```


You will be presented with an interactive console menu:
1. **Check Inbox:** Connects to the POP3 server, downloads new messages, saves them to SQLite, and deletes them from the server.
2. **Read Message:** Displays the list of locally saved messages and lets you read a specific one by ID.
3. **Compose and Send Email:** Prompts for recipient, subject, and multi-line body (end with a single `.` on a new line) and sends it via SMTP.
4. **View Local Messages:** Lists all messages currently stored in the local SQLite database.
5. **Exit:** Safely closes the application.

