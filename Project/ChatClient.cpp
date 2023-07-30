//
// ChatClient.cpp
// 2023-07-10
// Ivan Konishchev
//

#include "ChatClient.h"

ChatClient::ChatClient()
{
    _socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket_file_descriptor == -1)
    {
        std::cout << "Creation of Socket failed!" << std::endl;
        exit(1);
    }

    // Установим адрес сервера
    _serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта
    _serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    _serveraddress.sin_family = AF_INET;

    _connection = connect(_socket_file_descriptor, (struct sockaddr *)&_serveraddress, sizeof(_serveraddress));
    if (_connection == -1)
    {
        std::cout << "Connection with the server failed.!" << std::endl;
        exit(1);
    }
}

void ChatClient::close_client()
{
    char message[MESSAGE_LENGTH];
    bzero(message, MESSAGE_LENGTH);
    message[0] = 'e';
    message[1] = 'n';
    message[2] = 'd';
    message[3] = '\0';
    write(_socket_file_descriptor, message, MESSAGE_LENGTH);

    // закрываем сокет, завершаем соединение
    close(_socket_file_descriptor);
}
