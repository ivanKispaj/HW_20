//
// MessagesLogger.h
// 2023-08-01
// Ivan Konishchev
//

#pragma once
#include <fstream>
#include <string>
#include <shared_mutex>
#include <iostream>
#include "Message.h"
#include <thread>

class MessagesLogger
{
    std::fstream _file_handler;
    const char *_fileName = "../Desktop/ServerTCP/data/log.txt";
    std::shared_mutex _mutex;
    void saveLog(const std::string &logData);

public:
    MessagesLogger();
    ~MessagesLogger();
    void saveMessageLog(const Message &message);
    std::string loadMessageLogLine();
};