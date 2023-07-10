//
// DB.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "DB.h"
#include "EncodePassword.h"
namespace fs = std::filesystem;
void DB::saveToFileData()
{

    std::string usersfile = _fileBaseDir + _usersFile;
    std::string messagesfile = _fileBaseDir + _messagesFile;
    fs::path userstxt{usersfile};
    fs::path messagestxt{messagesfile};
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
        file_outU.close();
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
        file_outM.close();
    }

    fs::permissions(usersfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
    fs::permissions(messagesfile, fs::perms::group_all | fs::perms::owner_all | fs::perms::others_all, fs::perm_options::remove);
}

void DB::loadMessagesFromFile()
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

bool DB::loadUsersFromFile()
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

bool DB::addUser(User &user)
{
    if (isUniqueLogin(user.getUserLogin()))
    {
        if (user.getId() == -1)
        {
            user.setCurrentID();
        }
        _userDB->append(user);

        return true;
    }
    return false;
}

void DB::addMessage(Message &message)
{
    if (message.getAuthorID() != 0 && message.getRecipientID() != 0)
    {
        message.isPrivate = true;
        message.setDateMessage();
        message.setMessageId();
        _messageDB->append(message);
    }
}

void DB::AddMessageToAllUsers(Message &message)
{
    if (message.getAuthorID() != 0)
    {
        message.isPrivate = false;
        message.setDateMessage();
        message.setMessageId();
        _messageDB->append(message);
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

const std::unique_ptr<User[]> DB::getAllUsers() const
{
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

const std::unique_ptr<User> DB::getUserByLogin(const std::string &login, bool exception) const
{
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

const std::unique_ptr<User> DB::getUserById(const int &userID) const
{
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

int DB::usersCount() const
{
    return _userDB->count();
}

const std::unique_ptr<Message> DB::getMessage(const int &messageId) const
{
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

const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUsersById(int user1Id, int user2Id, int &size) const
{
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

const std::unique_ptr<Message[]> DB::getAllPrivateMessagesForUserById(int userId, int &size) const
{
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

const std::unique_ptr<Message[]> DB::getAllPublicMessagesForUserById(int Id, int &size) const
{
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

const std::unique_ptr<Message[]> DB::getAllPublicMessages(int &size) const
{

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