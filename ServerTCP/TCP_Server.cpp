//
// TCP_Server.cpp
// 2023-07-10
// Ivan Konishchev
//

#include "TCP_Server.h"
void TCP_Server::start_server()
{
    _sockert_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockert_file_descriptor < 0)
    {
        std::cout << "Socket creation failed.!" << std::endl;
        exit(1);
    }

    _serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    _serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    _serveraddress.sin_family = AF_INET;

    // Привяжем сокет
    _bind_status = bind(_sockert_file_descriptor, (struct sockaddr *)&_serveraddress,
                        sizeof(_serveraddress));
    if (_bind_status < 0)
    {
        std::cout << "Socket binding failed.!" << std::endl;
        exit(1);
    }

    // Поставим сервер на прием данных
    _connection_status = listen(_sockert_file_descriptor, 5);
    if (_connection_status == -1)
    {
        std::cout << "Socket is unable to listen for new connections.!" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Server is listening for new connection: " << std::endl;
    }
    _status = noConnectClient;
    _connectionThread = std::thread([&]()
                                    { this->connectionLoop(); });
    _waitingMessageThread = std::thread([&]()
                                        { this->waitingMessageLoop(); });
    this->joinLoop();
}

void TCP_Server::stop_server()
{
}

void TCP_Server::connectionLoop()
{
    while (1)
    {

        _length = sizeof(_client);
        _connection = accept(_sockert_file_descriptor, (struct sockaddr *)&_client, &_length);
        if (_connection == -1)
        {
            std::cout << "Server is unable to accept the data from client.!" << std::endl;
            exit(1);
        }
        std::cout << "connect client to server, client address: " << _connection << "\n";
        _connectMutex.lock();
        _connectionId.push_back(_connection);
        _connectMutex.unlock();
    }
}

void TCP_Server::waitingMessageLoop()
{
    while (1)
    {

        _connectMutex.lock();
        if ((_connectionId.size() == 0) && (_status == noConnectClient))
        {
            _status = start;
            std::cout << "All client disconected!\nWiting new connections\n";
             _connectMutex.unlock();
            continue;
        }
        std::vector<int> eraseClient;
        for (auto index = 0; index < _connectionId.size(); index++)
        {
            bzero(_message, MESSAGE_LENGTH);
            int answ = recv(_connectionId[index], _message, MESSAGE_LENGTH, MSG_DONTWAIT);
            if (answ != 0)
            {
                if (_message[0] == '\0')
                {
                    continue;
                }
                std::cout << "RECIVE DATA FROM CLIENT: \n";

                if (strncmp("end", _message, 3) == 0)
                {
                    eraseClient.push_back(_connectionId[index]);
                }
                else
                {
                    std::cout << "Count client: " << _connectionId.size() << "\n";
                    bzero(_message, MESSAGE_LENGTH);
                    _message[0] = 'O';
                    _message[1] = 'k';
                    _message[2] = '!';
                    _message[3] = '\0';
                    ssize_t bytes = write(_connectionId[index], _message, MESSAGE_LENGTH);

                    if (bytes >= 0)
                    {
                        std::cout << "Data successfully sent to the client.!" << std::endl;
                        std::cout << "Response " << bytes << " byte." << std::endl;
                    }
                }
            }
            else
            {
                _status = noConnectClient;
                eraseClient.push_back(_connectionId[index]);
            }
        }

        int count = eraseClient.size();
        int eraseindex = 0;
        while (count > 0)
        {
            for (auto it = _connectionId.begin(); it != _connectionId.end(); it++)
            {
                if (*it == eraseClient[eraseindex])
                {
                    _connectionId.erase(it);
                    count--;
                    break;
                }
            }
            break;
        }

        // if (_connectionId.size() == 0)
        // {
        //     _status = start;
        // }
        _connectMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void TCP_Server::setStatus()
{
}

void TCP_Server::joinLoop()
{
    _connectionThread.join();
    _waitingMessageThread.join();
}