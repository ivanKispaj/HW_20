//
// TCP_Server.cpp
// 2023-07-10
// Ivan Konishchev
//

#include "TCP_Server.h"

/// @brief Настройка всех переменных и старт сервера (старт потоков ожидания подключений и данных)
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

    if (!loadUsersFromFile())
    {
        std::cout << "Create admin and complain_bot users!\n";
        User service_admin("admin", "admin", "1234");
        service_admin.setIsAdmin(true);
        _userDB->append(service_admin);
        User service_bot("complaint_bot", "complaint_bot", "uijyh6758r9ifSDFG4rdfcvgtre"); // Service Admin userID = 1;
        _userDB->append(service_bot);
        std::cout << "Логин сервисного администратора: admin, пароль: 1234" << std::endl;
        std::cout << "Рекомендуем сменить пароль. Войдите в чат -> настройки профиля." << std::endl;
        std::cout << "Это сообщение больше не будет отображаться." << std::endl;
        std::cout << std::endl;
        std::cout << "Для остановки сервера введите -> 'q'\n";
        std::cout << std::endl;
        saveToFileData();
        
    }
    loadMessagesFromFile();
    _status = noConnectClient;
    _connectionThread = std::thread([&]()
                                    { this->connectionLoop(); });
    _waitingMessageThread = std::thread([&]()
                                        { this->waitingMessageLoop(); });
    std::thread exitLoop = std::thread([&]()
    {
        closeServerLoop();
    });
    exitLoop.join();
    this->joinLoop();
}

/// @brief поток ожидающий подключения клиентов и добавляющий их id подключения в массив
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

/// @brief Поток ожидающий данных от клиента
///         если ошибка получения от одного из клиентов значит он отклбчился и удаляем его из массива
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

        std::vector<int> eraseClient; // массив клиентов отключенных для удаления из массива подключенных
        for (auto index = 0; index < _connectionId.size(); index++)
        {
            char response[sizeof(CommandToServer)];
            bzero(response, sizeof(CommandToServer));
            int answ = recv(_connectionId[index], response, sizeof(CommandToServer), MSG_DONTWAIT);
            if (answ != 0)
            {
                CommandToServer command;
                memcpy((char *)&command, response, sizeof(CommandToServer));
                switch (command)
                {
                case ADD_USER:
                {
                    int sizeUserData{0};
                    // получаем размер юбинарных данных пользователя
                    answ = recv(_connectionId[index], (char *)&sizeUserData, sizeof(sizeUserData), MSG_DONTWAIT);

                    if (answ != 0)
                    {
                        char data[sizeUserData];
                        answ = recv(_connectionId[index], data, sizeUserData, MSG_DONTWAIT);
                        if (answ != 0)
                        {
                            User user;
                            user.parseFromBinaryData(data);
                            std::string login = user.getUserLogin();
                            if (_userDB->isUnique(login))
                            {
                                user.setCurrentID();
                                _userDB->append(user);

                                std::cout << "Added user with id: " << user.getId() << "\n";

                                send(_connectionId[index], (char *)&ServerRsponse::_true, ServerRsponse::_size, MSG_DONTWAIT);
                            }
                            else
                            {
                                send(_connectionId[index], (char *)&ServerRsponse::_false, ServerRsponse::_size, MSG_DONTWAIT);
                            }
                        }
                        else
                        {
                            clientDisconnected(eraseClient, _connectionId[index]);
                        }
                    }
                    else
                    {
                        clientDisconnected(eraseClient, _connectionId[index]);
                    }
                    saveToFileData();
                }
                break;

                case ADD_MESSAGE:

                {
                    int sizeData{0};
                    answ = recv(_connectionId[index], (char *)&sizeData, sizeof(sizeData), MSG_DONTWAIT);
                    if (answ != 0)
                    {
                        char data[sizeData];
                        answ = recv(_connectionId[index], data, sizeData, MSG_DONTWAIT);
                        if (answ != 0)
                        {
                            Message message;
                            message.parseFromBinaryData(data);
                            message.setDateMessage();
                            message.setMessageId();
                            _messageDB->append(message);
                        }
                        else
                        {
                            clientDisconnected(eraseClient, _connectionId[index]);
                        }
                    }
                    else
                    {
                        clientDisconnected(eraseClient, _connectionId[index]);
                    }
                    saveToFileData();
                }
                break;

                case IS_UNIQUE_LOGIN:
                {

                    int sizeUserData{0};
                    // получаем размер юбинарных данных пользователя
                    answ = recv(_connectionId[index], (char *)&sizeUserData, sizeof(sizeUserData), MSG_DONTWAIT);

                    if (answ != 0)
                    {
                        char data[sizeUserData];
                        answ = recv(_connectionId[index], data, sizeUserData, MSG_DONTWAIT);
                        std::string login;
                        login.resize(sizeUserData);
                        memcpy(&login[0], data, sizeUserData);
                        if (answ != 0)
                        {
                            if (_userDB->isUnique(login))
                            {
                                write(_connectionId[index], ServerRsponse::_true, ServerRsponse::_size);
                            }
                            else
                            {
                                write(_connectionId[index], ServerRsponse::_false, ServerRsponse::_size);
                            }
                        }
                        else
                        {
                            clientDisconnected(eraseClient, _connectionId[index]);
                        }
                    }
                    else
                    {
                        clientDisconnected(eraseClient, _connectionId[index]);
                    }
                }

                break;

                case GET_ALL_USERS:
                {

                    int usersCount{0};

                    std::unique_ptr<User[]> users = _userDB->getTableData(usersCount);
                    if (usersCount > 0)
                    {
                        char *responseData = (char *)std::malloc(usersCount);
                        bzero(responseData, usersCount);
                        memcpy(responseData, &usersCount, sizeof(usersCount));
                        int currentSize = sizeof(usersCount);
                        // Отправляем количество сообщений для получения
                        for (int i = 0; i < usersCount; i++)
                        {
                            int sizeData{0};
                            char *data = users[i].parseToBinaryData(sizeData);
                            char temp[currentSize + sizeof(sizeData) + sizeData];
                            bzero(temp, currentSize + sizeof(sizeData) + sizeData);
                            memcpy(temp, responseData, currentSize);
                            memcpy(temp + currentSize, &sizeData, sizeof(sizeData));
                            memcpy(temp + currentSize + sizeof(sizeData), data, sizeData);
                            currentSize += (sizeof(sizeData) + sizeData);
                            responseData = (char *)std::realloc(responseData, currentSize);
                            bzero(responseData, currentSize);
                            memcpy(responseData, temp, currentSize);
                            delete[] data;
                        }
                        send(_connectionId[index], responseData, currentSize, MSG_DONTWAIT);

                        std::free(responseData);
                    }
                    else
                    {
                        int size{0};
                        send(_connectionId[index], (char *)&size, sizeof(size), MSG_DONTWAIT);
                        //  write(_connectionId[index], &size, sizeof(size));
                    }
                }
                break;

                case GET_USER_BY_LOGIN:
                {
                    int sizeUserData{0};
                    // получаем размер юбинарных данных пользователя
                    answ = recv(_connectionId[index], (char *)&sizeUserData, sizeof(sizeUserData), MSG_DONTWAIT);

                    if (answ != 0)
                    {
                        char data[sizeUserData];
                        answ = recv(_connectionId[index], data, sizeUserData, MSG_DONTWAIT);
                        std::string login;
                        login.resize(sizeUserData);
                        memcpy(&login[0], data, sizeUserData);
                        if (answ != 0)
                        {
                            int count = 0;
                            std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);
                            bool isFindUser{false};
                            for (int i = 0; i < count; i++)
                            {
                                if (usersAll[i].getUserLogin() == login)
                                {
                                    // вурнуть пользователя и ответ true!!!
                                    int size{0};
                                    char *userBinary = usersAll[i].parseToBinaryData(size);
                                    char buffer[(size + ServerRsponse::_size + sizeof(size))];
                                    bzero(buffer, (size + ServerRsponse::_size + sizeof(size)));
                                    memcpy(buffer, ServerRsponse::_true, ServerRsponse::_size);
                                    memcpy(buffer + ServerRsponse::_size, &size, sizeof(size));
                                    memcpy(buffer + (ServerRsponse::_size + sizeof(size)), userBinary, size);
                                    write(_connectionId[index], buffer, (size + ServerRsponse::_size + sizeof(size)));
                                    isFindUser = true;
                                    break;
                                }
                            }

                            // вурнуть ответ false;
                            if (!isFindUser)
                            {
                                write(_connectionId[index], ServerRsponse::_false, ServerRsponse::_size);
                            }
                        }
                        else
                        {
                            clientDisconnected(eraseClient, _connectionId[index]);
                        }
                    }
                    else
                    {
                        clientDisconnected(eraseClient, _connectionId[index]);
                    }
                }
                break;

                case GET_USER_BY_ID:
                {
                    long long int id{-1};
                    // получаем размер юбинарных данных пользователя
                    answ = recv(_connectionId[index], &id, sizeof(id), MSG_DONTWAIT);

                    if (answ != 0)
                    {
                        int count = 0;
                        std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);
                        bool isFindUser{false};

                        for (int i = 0; i < count; i++)
                        {
                            if (usersAll[i].getId() == id)
                            {
                                // вурнуть пользователя и ответ true!!!
                                int size{0};
                                char *userBinary = usersAll[i].parseToBinaryData(size);
                                char buffer[(size + ServerRsponse::_size + sizeof(size))];
                                bzero(buffer, (size + ServerRsponse::_size + sizeof(size)));
                                memcpy(buffer, ServerRsponse::_true, ServerRsponse::_size);
                                memcpy(buffer + ServerRsponse::_size, &size, sizeof(size));
                                memcpy(buffer + (ServerRsponse::_size + sizeof(size)), userBinary, size);
                                write(_connectionId[index], buffer, (size + ServerRsponse::_size + sizeof(size)));
                                isFindUser = true;
                                break;
                            }
                        }

                        // вурнуть ответ false;
                        if (!isFindUser)
                        {
                            write(_connectionId[index], ServerRsponse::_false, ServerRsponse::_size);
                        }
                    }
                    else
                    {
                        clientDisconnected(eraseClient, _connectionId[index]);
                    }
                }
                break;

                case USERS_COUNT:
                {
                    int countUsers = _userDB->count();
                    char buffer[sizeof(countUsers)];
                    bzero(buffer, sizeof(countUsers));
                    memcpy(buffer, &countUsers, sizeof(countUsers));

                    write(_connectionId[index], buffer, sizeof(countUsers));
                }
                break;

                case GET_MESSAGE:
                    break;

                case UPDATE_USER_DATA:
                    break;

                case DELETE_MESSAGE_BY_ID:
                    break;

                case GET_ALL_MESSAGES:
                {
                    int messageCount{0};

                    std::unique_ptr<Message[]> messages = _messageDB->getTableData(messageCount);
                    if (messageCount > 0)
                    {
                        char *responseData = (char *)std::malloc(messageCount);
                        bzero(responseData, messageCount);
                        memcpy(responseData, &messageCount, sizeof(messageCount));
                        int currentSize = sizeof(messageCount);
                        // Отправляем количество сообщений для получения
                        for (int i = 0; i < messageCount; i++)
                        {
                            int sizeData{0};
                            char *data = messages[i].parseToBinaryData(sizeData);
                            char temp[currentSize + sizeof(sizeData) + sizeData];
                            bzero(temp, currentSize + sizeof(sizeData) + sizeData);
                            memcpy(temp, responseData, currentSize);
                            memcpy(temp + currentSize, &sizeData, sizeof(sizeData));
                            memcpy(temp + currentSize + sizeof(sizeData), data, sizeData);
                            currentSize += (sizeof(sizeData) + sizeData);
                            responseData = (char *)std::realloc(responseData, currentSize);
                            bzero(responseData, currentSize);
                            memcpy(responseData, temp, currentSize);
                            delete[] data;
                        }
                        send(_connectionId[index], responseData, currentSize, MSG_DONTWAIT);

                        std::free(responseData);
                    }
                    else
                    {
                        int size{0};
                        send(_connectionId[index], (char *)&size, sizeof(size), MSG_DONTWAIT);
                        //  write(_connectionId[index], &size, sizeof(size));
                    }
                }
                break;

                case EMPTY:
                    break;
                }
            }
            else
            {
                clientDisconnected(eraseClient, _connectionId[index]);
            }
        }

        // После опроса всех подключенных клиентов удаление тех кто отключился из списака подключенных

        for (auto itVec = eraseClient.begin(); itVec != eraseClient.end(); itVec++)
        {
            for (auto it = _connectionId.begin(); it != _connectionId.end(); it++)
            {
                if (*it == *itVec)
                {
                    _connectionId.erase(it);
                    break;
                }
            }
        }
        // разблокирока переменных и ожидание 50мс чтобы сильно не грузить систему.

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

void TCP_Server::saveMessageToFile(const char *message, int size)
{
}

int TCP_Server::nextServerAnswer(int connectId, char *response, int size)
{
    return recv(connectId, response, size, MSG_DONTWAIT);
}

void TCP_Server::clientDisconnected(std::vector<int> &eraseClient, int connectId)
{
    _status = noConnectClient;
    eraseClient.push_back(connectId);
}

namespace fs = std::filesystem;

void TCP_Server::saveToFileData()
{
    std::string usersfile = _fileBaseDir + _usersFile;
    std::string messagesfile = _fileBaseDir + _messagesFile;
    fs::path userstxt{usersfile};
    fs::path messagestxt{messagesfile};
    fs::path directory(_fileBaseDir);

    if (!fs::exists(directory))
    {
        fs::create_directories(directory);
    }
    if (fs::exists(userstxt) && fs::exists(messagestxt))
    {
        fs::permissions(usersfile, fs::perms::owner_write, fs::perm_options::add);
        fs::permissions(messagesfile, fs::perms::owner_write, fs::perm_options::add);
    }

    std::fstream file_outU(usersfile, std::ios::out | std::ios::trunc);
    std::fstream file_outM(messagesfile, std::ios::out | std::ios::trunc);

    if (_userDB->count() > 0)
    {

        int count = 0;
        std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);
        for (int i = 0; i < count; i++)
        {
            file_outU << usersAll[i] << std::endl;
        }
    }

    if (_messageDB->count() > 0)
    {
        fs::permissions(messagesfile, fs::perms::owner_write, fs::perm_options::add);

        int count = 0;
        std::unique_ptr<Message[]> messages = _messageDB->getTableData(count);
        for (int i = 0; i < count; i++)
        {
            file_outM << messages[i] << std::endl;
        }
    }

    file_outU.close();
    file_outM.close();
    fs::permissions(usersfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
    fs::permissions(messagesfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
}

void TCP_Server::loadMessagesFromFile()
{
    std::string messagesfile = _fileBaseDir + _messagesFile;
    fs::path messagestxt{messagesfile};
    if (fs::exists(messagestxt))
    {
        fs::permissions(messagesfile, fs::perms::owner_read, fs::perm_options::add);

        std::fstream file_readM(messagesfile, std::ios::in);

        if (file_readM.is_open())
        {
            Message message;
            while (file_readM >> message)
            {
                message._currentId++;
                _messageDB->append(message);
            }
        }
        file_readM.close();
        fs::permissions(messagesfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
    }
}

bool TCP_Server::loadUsersFromFile()
{
    std::string usersfile = _fileBaseDir + _usersFile;
    fs::path userstxt{usersfile};
    if (fs::exists(userstxt))
    {
        fs::permissions(usersfile, fs::perms::owner_read, fs::perm_options::add);
        std::fstream file_readU{usersfile, std::ios::in};
        if (file_readU.is_open())
        {
            User user;
            bool ret{false};
            while (file_readU >> user)
            {
                user._currentId++;
                _userDB->append(user);
                ret = true;
            }
            file_readU.close();
            fs::permissions(usersfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
            return ret;
        }
    }
    return false;
}

void TCP_Server::closeServerLoop()
{
    while(true)
    {
        char exit;
        std::cin >> exit;
        if (exit == 'q')
        {
            break;
        }
    }
    std::cout << "server is stoped!\n";
    close(_sockert_file_descriptor);
    exit(0);
}