//
// DB.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "DB.h"
#include "EncodePassword.h"

bool DB::addUser(User &user)
{
    _command = ADD_USER;
    int size{0};
    char *binaryData = user.parseToBinaryData(size);
    char *sendData = prepareBinaryQueryData(binaryData, size);
    _server.sendToServer(_command, false, sendData, size + sizeof(size));
    delete[] binaryData;
    delete[] sendData;

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
        char *sendData = prepareBinaryQueryData(binaryData, size);
        _server.sendToServer(_command,false, sendData, size + sizeof(size));
        delete[] binaryData;
        delete[] sendData;     
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
        char *sendData = prepareBinaryQueryData(binaryData, size);
        _server.sendToServer(_command,false, sendData, size + sizeof(size));

        delete[] binaryData;
        delete[] sendData;
        loadMessagesFromServer();
    }
}


bool DB::setUserPassword(int userID, const std::string &pass)
{
    if (std::unique_ptr<User> user = _userDB->getDataById(userID))
    {
        (*user).setUserPassword(pass);
        if(updateUserData(*user))
        {
            _userDB->updateData(*user);
            return true;
        }
        
    }
    return false;
}

bool DB::isUniqueLogin(const std::string &login)
{
    _command = IS_UNIQUE_LOGIN;

    int size = login.length() * sizeof(login[0]);
    char *sendData = new char[size + sizeof(size)];
    bzero(sendData, size + sizeof(size));
    memcpy(sendData, &size, sizeof(size));
    memcpy(sendData + sizeof(size), &login[0], size);
    _server.sendToServer(_command,false, sendData,size + sizeof(size));
    delete[] sendData;
    
    if(_server.fetchBoolResponse())
    {
        return true;
    }
    return false;
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
            EncodePassword::sha1(verifyPass);
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
   // loadUsersFromServer();
   _command = USERS_COUNT;
   _server.sendToServer(_command,true);
   
    return _server.fetchIntResponse();
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


bool DB::deleteMessageById(int Id)
{
    _command = DELETE_MESSAGE_BY_ID;
    char sendData[sizeof(Id)];
    bzero(sendData,sizeof(Id));
    memcpy(sendData, &Id, sizeof(Id));
    _server.sendToServer(_command, false, sendData, sizeof(Id));

    return true;
}


bool DB::updateUserData(User &user)
{
    _command = UPDATE_USER_DATA;
    int size{0};
    char *binaryData = user.parseToBinaryData(size);
    char *sendData = prepareBinaryQueryData(binaryData, size);
    _server.sendToServer(_command, false, sendData, size + sizeof(size));
    
    delete[] binaryData;
    delete[] sendData;
    //_server.fetchBinaryData()
    return true;
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

char *DB::prepareBinaryQueryData(char * binaryData,int size)
{
    char *sendData = new char[size + sizeof(size)];
    bzero(sendData, size + sizeof(size));
    memcpy(sendData, &size, sizeof(size));
    memcpy(sendData + sizeof(size), binaryData, size);
    return sendData;
}