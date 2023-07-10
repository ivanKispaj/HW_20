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
#define MESSAGE_LENGTH 4096 // Максимальный размер буфера для данных
#define PORT 45000          // Будем использовать этот номер порта

class TCP_Server
{

    
    enum Status : uint8_t
    {
        start = 0,
        stop = 1,
        
        noConnectClient = 4

    };

    Status _status;
    std::thread _connectionThread;
    std::thread _waitingMessageThread;

    int _sockert_file_descriptor{0}, _connection{0}, _bind_status{0}, _connection_status{0};
    char _message[MESSAGE_LENGTH];
    struct sockaddr_in _serveraddress, _client;
    socklen_t _length;

    std::mutex _connectMutex;
    std::vector<int> _connectionId;

    void connectionLoop();
    void waitingMessageLoop();
    void setStatus();
    void joinLoop();

public:
    TCP_Server() = default;

    void start_server();
    void stop_server();
};
