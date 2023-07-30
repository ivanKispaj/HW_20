//
// DB.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "DB.h"
#include "EncodePassword.h"

// bool DB::addUser(User &user)
// {
//     _command = ADD_USER;
//     int size{0};
//     char *_binaryData = user.parseToBinaryData(size);
//     _server.sendToServer(_command, _binaryData, size);
//     delete[] _binaryData;
//     _binaryData = nullptr;

//     bool result = _server.fetchBoolResponse();
//     return result;
// }

// // private chat message
// void DB::addMessage(Message &message)
// {
//     _command = ADD_MESSAGE;
//     message.isPrivate = true;
//     message.setDateMessage();
//     message.setMessageId();
//     int size{0};
//     char *_binaryData = message.parseToBinaryData(size);
//     _server.sendToServer(_command, _binaryData, size);
//     delete[] _binaryData;
//     _binaryData = nullptr;
// }

// // public chat message
// void DB::AddMessageToAllUsers(Message &message)
// {
//     _command = ADD_MESSAGE;
//     message.isPrivate = false;
//     message.setDateMessage();
//     message.setMessageId();
//     int size{0};
//     char *_binaryData = message.parseToBinaryData(size);
//     _server.sendToServer(_command, _binaryData, size);
//     delete[] _binaryData;
//     _binaryData = nullptr;
// }

// bool DB::setUserPassword(int userID, const std::string &pass)
// {
//     if (std::unique_ptr<User> user = _userDB->getDataById(userID))
//     {
//         (*user).setUserPassword(pass);
//         _userDB->updateData(*user);
//     }
//     return false;
// }

// bool DB::isUniqueLogin(const std::string &login)
// {
//     _command = IS_UNIQUE_LOGIN;
//     char *_binaryData = new char[login.length()];
//     bzero(_binaryData, login.length());
//     memcpy(_binaryData, &login[0], login.length());
//     _server.sendToServer(_command, _binaryData, login.length());
//     delete[] _binaryData;
//     _binaryData = nullptr;
//     bool response = _server.fetchBoolResponse();
//     return response;
// }

// bool DB::isCorrectPassword(int userID, const std::string &pass)
// {
//     int count = 0;
//     std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);

//     for (int i = 0; i < count; i++)
//     {
//         if (usersAll[i].getId() == userID)
//         {
//             std::string verifyPass = pass;
//             EncodePassword::encodePassword(verifyPass);
//             if (usersAll[i].getUserPassword() == verifyPass)
//             {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// const std::unique_ptr<User[]> DB::getAllUsers()
// {
//     loadUsersFromServer();
//     int count = 0;
//     std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);
//     if (count > 0)
//     {
//         return usersAll;
//     }
//     return nullptr;
// }

// const std::unique_ptr<User> DB::getUserByLogin(const std::string &login, bool exception)
// {
//     _command = GET_USER_BY_LOGIN;
//     char *binaryData = new char[login.length()];
//     bzero(binaryData, login.length());
//     memcpy(binaryData, &login[0], login.length());
//     _server.sendToServer(_command, binaryData, login.length());
//     delete[] binaryData;
//     binaryData = nullptr;

//     std::unique_ptr<User> user = _server.fetchUserResponse();
//     if (user)
//     {
//         _userDB->append((*user));
//         return user;
//     }
//     else if (exception)
//     {
//         throw UserNotFoundException();
//     }
//     return nullptr;
// }

// const std::unique_ptr<User> DB::getUserById(const int &userID)
// {

//     _command = GET_USER_BY_ID;
//     long long int id = userID;
//     char binaryData[sizeof(id)];
//     bzero(binaryData, sizeof(id));
//     memcpy(binaryData, &id, sizeof(id));
//     _server.sendToServer(_command, binaryData, sizeof(id));
//     std::unique_ptr<User> user = _server.fetchUserResponse();
//     if (user)
//     {
//         _userDB->append((*user));
//         return user;
//     }
//     return nullptr;
// }

// int DB::usersCount()
// {
//     _command = USERS_COUNT;
//     char data[1];
//     _server.sendToServer(_command, data, 1);
//     return _server.fetchIntResponse();
// }

// const std::unique_ptr<Message> DB::getMessage(const int &messageId) const
// {
//     if (_messageDB->count() > 0)
//     {
//         int messSize = 0;
//         std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);

//         for (int i = 0; i < messSize; i++)
//         {
//             if (messagesAll[i].getId() == messageId)
//             {
//                 return std::make_unique<Message>(messagesAll[i]);
//             }
//         }
//     }
//     return nullptr;
// }

// bool DB::updateUserData(const User &user)
// {
//     if (_userDB->count() > 0)
//     {
//         User userUpdate = user;
//         _userDB->updateData(userUpdate);
//     }
//     return false;
// }

// bool DB::deleteMessageById(int Id)
// {
//     if (_messageDB->count() > 0)
//     {
//         _messageDB->deleteById(Id);
//         return true;
//     }
//     return false;
// }

// const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUsersById(int user1Id, int user2Id, int &size)
// {
//     loadMessagesFromServer();
//     if (_messageDB->count() > 0)
//     {
//         int messSize = 0;
//         std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);

//         std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);
//         int index = 0;
//         for (int i = 0; i < messSize; i++)
//         {
//             if (messagesAll[i].isPrivate)
//             {
//                 if ((messagesAll[i].getRecipientID() == user2Id && messagesAll[i].getAuthorID() == user1Id) ||
//                     (messagesAll[i].getRecipientID() == user1Id && messagesAll[i].getAuthorID() == user2Id))
//                 {
//                     tempArr[index] = messagesAll[i];
//                     index++;
//                 }
//             }
//         }
//         std::unique_ptr<Message[]> ret(new Message[index]);

//         for (int i = 0; i < index; i++)
//         {
//             ret[i] = tempArr[i];
//         }
//         size = index;
//         return ret;
//     }
//     size = 0;
//     return nullptr;
// }

// const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUserById(int userId, int &size)
// {
//     loadMessagesFromServer();

//     if (_messageDB->count() > 0)
//     {
//         int messSize = 0;
//         std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
//         std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

//         int index = 0;
//         for (int i = 0; i < messSize; i++)
//         {
//             if (messagesAll[i].isPrivate && messagesAll[i].getRecipientID() == userId)
//             {

//                 tempArr[index] = messagesAll[i];
//                 index++;
//             }
//         }
//         std::unique_ptr<Message[]> ret(new Message[index]);

//         for (int i = 0; i < index; i++)
//         {
//             ret[i] = tempArr[i];
//         }
//         size = index;
//         return ret;
//     }
//     size = 0;
//     return nullptr;
// }

// const std::unique_ptr<Message[]> DB::getAllPublicMessagesForUserById(int Id, int &size)
// {
//     loadMessagesFromServer();
//     if (_messageDB->count() > 0)
//     {
//         int messSize = 0;
//         std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
//         std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

//         int index = 0;
//         for (int i = 0; i < messSize; i++)
//         {
//             if (!messagesAll[i].isPrivate)
//             {
//                 if (messagesAll[i].getAuthorID() == Id || messagesAll[i].getRecipientID() == Id)
//                 {
//                     tempArr[index] = messagesAll[i];
//                     index++;
//                 }
//             }
//         }
//         std::unique_ptr<Message[]> ret(new Message[index]);

//         for (int i = 0; i < index; i++)
//         {
//             ret[i] = tempArr[i];
//         }
//         size = index;
//         return ret;
//     }
//     size = 0;
//     return nullptr;
// }

// const std::unique_ptr<Message[]> DB::getAllPublicMessages(int &size)
// {

//     loadMessagesFromServer();
//     if (_messageDB->count() > 0)
//     {
//         std::cout << "GetAllPublicMessages after load from server\n";
//         int messSize = 0;
//         std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
//         std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

//         int index = 0;
//         for (int i = 0; i < messSize; i++)
//         {
//             if (!messagesAll[i].isPrivate)
//             {
//                 tempArr[index] = messagesAll[i];
//                 index++;
//             }
//         }
//         std::unique_ptr<Message[]> ret(new Message[index]);

//         for (int i = 0; i < index; i++)
//         {
//             ret[i] = tempArr[i];
//         }
//         size = index;
//         std::cout << "messages return: " << messSize << "\n";
//         std::cout << "return count: " << index << "\n";
//         return ret;
//     }
//     size = 0;
//     return nullptr;
// }
bool DB::addUser(User &user)
{
    _command = ADD_USER;
    int size{0};
    char *binaryData = user.parseToBinaryData(size);
    char sendData[size + sizeof(size)];
    // передаем размер данных и сами данные!
    bzero(sendData, size + sizeof(size));
    memcpy(sendData, &size, sizeof(size));
    memcpy(sendData + sizeof(size), binaryData, size);
    _server.sendToServer(_command, false, sendData, size + sizeof(size));

    delete[] binaryData;
    binaryData = nullptr;

    bool result = _server.fetchBoolResponse();
    if (result)
    {
        loadUsersFromServer();
    }
    return result;
}

void DB::addMessage(Message &message)
{
    if (message.getAuthorID() != 0 && message.getRecipientID() != 0)
    {
        _command = ADD_MESSAGE;
        message.isPrivate = true;

        int size{0};
        char *binaryData = message.parseToBinaryData(size);
        char sendData[size + sizeof(size)];
        // передаем размер данных и сами данные!
        bzero(sendData, size + sizeof(size));
        memcpy(sendData, &size, sizeof(size));
        memcpy(sendData + sizeof(size), binaryData, size);

        _server.sendToServer(_command,false, sendData, size + sizeof(size));
        delete[] binaryData;
        binaryData = nullptr;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        loadMessagesFromServer();
    }
}

void DB::AddMessageToAllUsers(Message &message)
{
    if (message.getAuthorID() != 0)
    {
        _command = ADD_MESSAGE;
        message.isPrivate = false;

        int size{0};
        char *binaryData = message.parseToBinaryData(size);
        char sendData[size + sizeof(size)];
        // передаем размер данных и сами данные!
        bzero(sendData, size + sizeof(size));
        memcpy(sendData, &size, sizeof(size));
        memcpy(sendData + sizeof(size), binaryData, size);

        // отправляет на сервер данные ( (int) размер данных + данные)
        _server.sendToServer(_command,false, sendData, size + sizeof(size));

        delete[] binaryData;
        binaryData = nullptr;
        loadMessagesFromServer();
    }
}

bool DB::setUserPassword(int userID, const std::string &pass)
{
    if (std::unique_ptr<User> user = _userDB->getDataById(userID))
    {
        (*user).setUserPassword(pass);
        _userDB->updateData(*user);
    }
    return false;
}

bool DB::isUniqueLogin(const std::string &login)
{
    loadUsersFromServer();

    int count = 0;
    std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);

    for (int i = 0; i < count; i++)
    {
        if (usersAll[i].getUserLogin() == login)
        {
            return false;
        }
    }
    return true;
}

bool DB::isCorrectPassword(int userID, const std::string &pass)
{
    loadUsersFromServer();

    int count = 0;
    std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);

    for (int i = 0; i < count; i++)
    {
        if (usersAll[i].getId() == userID)
        {
            std::string verifyPass = pass;
            EncodePassword::encodePassword(verifyPass);
            if (usersAll[i].getUserPassword() == verifyPass)
            {
                return true;
            }
        }
    }
    return false;
}

const std::unique_ptr<User[]> DB::getAllUsers()
{
    loadUsersFromServer();

    if (_userDB->count() > 0)
    {
        int count = 0;
        std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);

        std::unique_ptr<User[]> ret(new User[count]);
        for (int i = 0; i < count; i++)
        {
            ret[i] = usersAll[i];
        }
        return ret;
    }
    return nullptr;
}

const std::unique_ptr<User> DB::getUserByLogin(const std::string &login, bool exception)
{
    loadUsersFromServer();

    if (_userDB->count() > 0)
    {
        int count = 0;
        std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);

        std::unique_ptr<User> user(new User());
        for (int i = 0; i < count; i++)
        {
            if (usersAll[i].getUserLogin() == login)
            {
                (*user) = usersAll[i];
                return user;
            }
        }
    }
    if (exception)
    {
        throw UserNotFoundException();
    }
    return nullptr;
}

const std::unique_ptr<User> DB::getUserById(const int &userID)
{
    loadUsersFromServer();
    if (_userDB->count() > 0)
    {
        int count = 0;
        std::unique_ptr<User[]> usersAll = _userDB->getTableData(count);
        std::unique_ptr<User> user(new User());

        for (int i = 0; i < count; i++)
        {
            if (usersAll[i].getId() == userID)
            {
                (*user) = usersAll[i];
                return user;
            }
        }
    }
    return nullptr;
}

int DB::usersCount()
{
    loadUsersFromServer();
    return _userDB->count();
}

const std::unique_ptr<Message> DB::getMessage(const int &messageId)
{
    loadMessagesFromServer();

    if (_messageDB->count() > 0)
    {
        int messSize = 0;
        std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);

        for (int i = 0; i < messSize; i++)
        {
            if (messagesAll[i].getId() == messageId)
            {
                return std::make_unique<Message>(messagesAll[i]);
            }
        }
    }
    return nullptr;
}

bool DB::updateUserData(const User &user)
{
    if (_userDB->count() > 0)
    {
        User userUpdate = user;
        _userDB->updateData(userUpdate);
    }
    return false;
}

bool DB::deleteMessageById(int Id)
{
    if (_messageDB->count() > 0)
    {
        _messageDB->deleteById(Id);
        return true;
    }
    return false;
}

const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUsersById(int user1Id, int user2Id, int &size)
{
    loadMessagesFromServer();

    if (_messageDB->count() > 0)
    {
        int messSize = 0;
        std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);

        std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);
        int index = 0;
        for (int i = 0; i < messSize; i++)
        {
            if (messagesAll[i].isPrivate)
            {
                if ((messagesAll[i].getRecipientID() == user2Id && messagesAll[i].getAuthorID() == user1Id) ||
                    (messagesAll[i].getRecipientID() == user1Id && messagesAll[i].getAuthorID() == user2Id))
                {
                    tempArr[index] = messagesAll[i];
                    index++;
                }
            }
        }
        std::unique_ptr<Message[]> ret(new Message[index]);

        for (int i = 0; i < index; i++)
        {
            ret[i] = tempArr[i];
        }
        size = index;
        return ret;
    }
    size = 0;
    return nullptr;
}

const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUserById(int userId, int &size)
{
    loadMessagesFromServer();

    if (_messageDB->count() > 0)
    {
        int messSize = 0;
        std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
        std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

        int index = 0;
        for (int i = 0; i < messSize; i++)
        {
            if (messagesAll[i].isPrivate && messagesAll[i].getRecipientID() == userId)
            {

                tempArr[index] = messagesAll[i];
                index++;
            }
        }
        std::unique_ptr<Message[]> ret(new Message[index]);

        for (int i = 0; i < index; i++)
        {
            ret[i] = tempArr[i];
        }
        size = index;
        return ret;
    }
    size = 0;
    return nullptr;
}

const std::unique_ptr<Message[]> DB::getAllPublicMessagesForUserById(int Id, int &size)
{
    loadMessagesFromServer();

    if (_messageDB->count() > 0)
    {
        int messSize = 0;
        std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
        std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

        int index = 0;
        for (int i = 0; i < messSize; i++)
        {
            if (!messagesAll[i].isPrivate)
            {
                if (messagesAll[i].getAuthorID() == Id || messagesAll[i].getRecipientID() == Id)
                {
                    tempArr[index] = messagesAll[i];
                    index++;
                }
            }
        }
        std::unique_ptr<Message[]> ret(new Message[index]);

        for (int i = 0; i < index; i++)
        {
            ret[i] = tempArr[i];
        }
        size = index;
        return ret;
    }
    size = 0;
    return nullptr;
}

const std::unique_ptr<Message[]> DB::getAllPublicMessages(int &size)
{
    loadMessagesFromServer();
    if (_messageDB->count() > 0)
    {
        int messSize = 0;
        std::unique_ptr<Message[]> messagesAll = _messageDB->getTableData(messSize);
        std::unique_ptr<Message[]> tempArr(new Message[_messageDB->count()]);

        int index = 0;
        for (int i = 0; i < messSize; i++)
        {
            if (!messagesAll[i].isPrivate)
            {
                tempArr[index] = messagesAll[i];
                index++;
            }
        }
        std::unique_ptr<Message[]> ret(new Message[index]);

        for (int i = 0; i < index; i++)
        {
            ret[i] = tempArr[i];
        }
        size = index;
        return ret;
    }
    size = 0;
    return nullptr;
}

void DB::loadMessagesFromServer()
{

    _command = GET_ALL_MESSAGES;
    _server.sendToServer(_command,true);

    int messageCount = _server.fetchIntResponse();
    if (messageCount > 0)
    {
        _messageDB->clearData();
        Message message;
        for (int i = 0; i < messageCount; i++)
        {

            int sizeBinData = _server.fetchIntResponse();

            char *data = _server.fetchBinaryData(sizeBinData);
            message.parseFromBinaryData(data);
            _messageDB->append(message);
   
            delete[] data;
        }
    }
}

void DB::loadUsersFromServer()
{
    

    _command = GET_ALL_USERS;
    _server.sendToServer(_command,true);

   
    int usersCounts = _server.fetchIntResponse();
    if (usersCounts > 0)
    {
        _userDB->clearData();
        User user;
        for (int i = 0; i < usersCounts; i++)
        {
            int sizeBinData = _server.fetchIntResponse();

            char *data = _server.fetchBinaryData(sizeBinData);
            user.parseFromBinaryData(data);
            _userDB->append(user);
            delete[] data;
        }
    }
}