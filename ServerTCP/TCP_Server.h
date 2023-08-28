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
#include "DBCoreMysql.h"
#include <stdlib.h>
//#include "MessagesLogger.h"

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

    int _port{45000};
    const char *_send = "send";
    const char *_received = "received";
    Status _status = stop;

    std::thread _connectionThread;
    std::thread _exitLoop;

    int _sockert_file_descriptor{0}, _connection{0}, _bind_status{0}, _connection_status{0};
    struct sockaddr_in _serveraddress, _client;
    socklen_t _length;

    std::mutex _connectMutex;
    std::vector<int> _connectionId;

    IDBCore<User> *_userDB = new DBCoreMysql<User>("users");
    IDBCore<Message> *_messageDB = new DBCoreMysql<Message>("messages");

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
    ~TCP_Server() = default;
    void start_server();
    void createDefaultUser();
    void configureServer();
};
