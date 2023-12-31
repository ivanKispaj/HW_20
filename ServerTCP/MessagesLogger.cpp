//
// MessagesLogger.cpp
// 2023-08-01
// Ivan Konishchev
//

#include "MessagesLogger.h"

MessagesLogger::MessagesLogger()
{
    _file_handler.open(_fileName, std::ios::out | std::ios::in | std::ios::app);
}

MessagesLogger::~MessagesLogger()
{
    _file_handler.close();
}

void MessagesLogger::saveMessageLog(const Message &message)
{
    if (_file_handler.is_open())
    {
        std::string logData = std::to_string(message.getDate()) + " -> " + message.getMessage();
        std::thread task(&MessagesLogger::saveLog, this, std::ref(logData));
        task.detach();
        // std::thread task = std::thread([&]()
        //                                {
        // _mutex.lock();
        // _file_handler.seekg(0, std::ios::end);
        // _file_handler << message.getDate() << " -> " << message.getMessage() << std::endl;
        // _mutex.unlock(); });
        // task.join();
    }
    else
    {
        std::cout << "File logger don open!\n";
    }
}

std::string MessagesLogger::loadMessageLogLine()
{
    std::string result;
    if (_file_handler.is_open())
    {

        std::thread task = std::thread([&]()
                                       {
                     _mutex.lock_shared();
                    _file_handler.seekg(0, std::ios::beg);          
                    std::getline(_file_handler, result);
                    _mutex.unlock_shared(); });
        task.detach();
    }
    return result;
}

void MessagesLogger::saveLog(const std::string &logData)
{
    _mutex.lock();
    _file_handler.seekg(0, std::ios::end);
    _file_handler << logData << std::endl;
    // _mutex.unlock();
};