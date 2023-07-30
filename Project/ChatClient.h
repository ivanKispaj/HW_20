//
// ChatClient.h
// 2023-07-10
// Ivan Konishchev
//

#pragma once
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include "Message.h"
#include <thread>
#include <mutex>
#include "User.h"
#include <fstream>
#include <thread>

#define MESSAGE_LENGTH 10 // Максимальный размер буфера для данных
#define PORT 45000
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

class ChatClient
{

    class ServerRsponse
    {
        ServerRsponse() = default;

    public:
        static constexpr const char *_true = "true";
        static constexpr const char *_false = "false";
        static constexpr int _size{6};
    };

    int _socket_file_descriptor, _connection;
    struct sockaddr_in _serveraddress, _client;

public:
    ChatClient();
    void close_client();

    void sendToServer(CommandToServer command,  bool onliCommand, const char *data = nullptr, int dataSize = 0)
    {
        int size = sizeof(command);
        char commandData[size];
        bzero(commandData,size);
        memcpy(commandData, &command, size);
        char *dataSend = nullptr;
        if (onliCommand)
        {
            dataSend = new char[size];
            bzero(dataSend,size);
            memcpy(dataSend, commandData, size);
        }
        else
        {
            size += (dataSize);
            int shift{0};
            dataSend = new char[size];
            bzero(dataSend, size);
            memcpy(dataSend, &command, sizeof(command));
            shift += sizeof(command);
            memcpy(dataSend + shift, data, dataSize);

            
        }

        send(_socket_file_descriptor, dataSend, size, MSG_WAITALL);
        delete[] dataSend;
    }

    bool fetchBoolResponse()
    {

        char response[ServerRsponse::_size];
        bzero(response, ServerRsponse::_size);
        recv(_socket_file_descriptor, response, ServerRsponse::_size, MSG_WAITALL);
        // read(_socket_file_descriptor, response, 6);
        if ((strncmp(response, "true", 4) == 0))
        {
            return true;
        }
        return false;
    }

    std::unique_ptr<User> fetchUserResponse()
    {
        char response[ServerRsponse::_size];
        bzero(response, ServerRsponse::_size);
        recv(_socket_file_descriptor, response, ServerRsponse::_size, MSG_WAITALL);
        // read(_socket_file_descriptor, response, 6);
        if ((strncmp(response, "true", 4) == 0))
        {

            int size{0};
            recv(_socket_file_descriptor, (char *)&size, sizeof(size), MSG_WAITALL);
            // read(_socket_file_descriptor, &size, sizeof(size));
            char data[size];
            std::unique_ptr<User> user(new User());
            recv(_socket_file_descriptor, data, size, MSG_WAITALL);
            // read(_socket_file_descriptor, data, size);
            user->parseFromBinaryData(data);
            return user;
        }
        return nullptr;
    }

    int fetchIntResponse()
    {
        int result{0};
        recv(_socket_file_descriptor, (char *)&result, sizeof(result), MSG_WAITALL);
        // read(_socket_file_descriptor, &result, sizeof(result));
        return result;
    }

    char *fetchBinaryData(int size)
    {
        if (size > 0)
        {
            char *temp = new char[size];
            recv(_socket_file_descriptor, temp, size, MSG_WAITALL);
            // read(_socket_file_descriptor, temp, size);
            return temp;
        }
        return nullptr;
    }
};