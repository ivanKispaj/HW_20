//
// TCP_Server.h
// 2023-07-10
// Ivan Konishchev
//

#pragma once
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <fstream>
#include <filesystem>
#include "DBCoreMap.h"
#include "User.h"
#include "Message.h"
#include <stdlib.h>

#define MESSAGE_LENGTH 4096 // Максимальный размер буфера для данных
#define PORT 45000          // Будем использовать этот номер порта

class TCP_Server
{

    class ServerRsponse
    {
        ServerRsponse() = default;

    public:
        static constexpr const char *_true = "true";
        static constexpr const char *_false = "false";
        static constexpr int _size{6};
    };

    enum CommandToServer : uint16_t
    {
        ADD_USER = 2,
        ADD_MESSAGE = 4,
        IS_UNIQUE_LOGIN = 8,
        GET_ALL_USERS = 16,
        GET_USER_BY_LOGIN = 32,
        GET_USER_BY_ID = 64,
        USERS_COUNT = 128,
        GET_MESSAGE = 256,
        UPDATE_USER_DATA = 512,
        DELETE_MESSAGE_BY_ID = 1024,
        GET_ALL_MESSAGES = 2048,
        EMPTY = 4096

    };

    enum Status : uint8_t
    {
        start = 0,
        stop = 1,
        noConnectClient = 2

    };

    std::string _fileBaseDir = "./bin/";
    std::string _usersFile = "Users.txt";
    std::string _messagesFile = "Messages.txt";

    const char *_send = "send";
    const char *_received = "received";
    Status _status = stop;

    std::thread _connectionThread;
    std::thread _waitingMessageThread;

    int _sockert_file_descriptor{0}, _connection{0}, _bind_status{0}, _connection_status{0};
    struct sockaddr_in _serveraddress, _client;
    socklen_t _length;

    std::mutex _connectMutex;
    std::vector<int> _connectionId;
    IDBCore<User> *_userDB = new DBCoreMap<User>();
    IDBCore<Message> *_messageDB = new DBCoreMap<Message>();

    void connectionLoop();
    void waitingMessageLoop();
    void closeServerLoop();
    void setStatus();
    void joinLoop();
    void saveMessageToFile(const char *message, int size);
    int nextServerAnswer(int connectId, char *response, int size);
    void clientDisconnected(std::vector<int> &eraseClient, int connectId);

public:
    TCP_Server() = default;

    void start_server();
    void saveToFileData();
    void loadMessagesFromFile();
    bool loadUsersFromFile();
};
