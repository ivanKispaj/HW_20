
//
// User.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "User.h"
#include "DB.h"
#include "EncodePassword.h"
long long int User::_currentId;

// Public methods

User::User(const std::string &name, const std::string &login, const std::string &pass) : _name(name), _login(login)
{
    setUserPassword(pass);
    setCurrentID();
}

long long int User::getId() const
{
    return _id;
}

const std::string &User::getUserName() const
{
    return _name;
}

const std::string &User::getUserLogin() const
{
    return _login;
}

const std::string &User::getUserPassword() const
{
    return _pass;
}

void User::setUserName(const std::string &name)
{
    _name = name;
}

void User::setUserLogin(const std::string &login)
{
    _login = login;
}

int User::getMessagesCount() const
{
    return _messageCount;
}

bool User::isAdmin() const
{
    return _isAdmin;
}

bool User::isBanned() const
{
    return _isBanned;
}

bool User::isDeleted() const
{
    return _isDeleted;
}

void User::setIsAdmin(bool isAdmin)
{
    _isAdmin = isAdmin;
}

void User::setIsBanned(bool isBanned)
{
    _isBanned = isBanned;
}

void User::deleteThisData()
{
    _name = "deleted";
    std::string pass = "DeLeTeD";
    EncodePassword::encodePassword(pass);
    _pass = pass;
    _isAdmin = false;
    _isBanned = false;
    _isDeleted = true;
}

User &User::operator=(const User &user)
{
    _name = user._name;
    _login = user._login;
    _pass = user._pass;
    _messageCount = user._messageCount;
    _id = user._id;
    _isAdmin = user._isAdmin;
    _isBanned = user._isBanned;
    _isDeleted = user._isDeleted;
    return *this;
}

// Private methods for Friends "DB"

void User::setUserPassword(const std::string &pass)
{
    std::string passWord = pass;
    EncodePassword::encodePassword(passWord);
    _pass = passWord;
}

void User::copyUserPassword(const std::string &pass)
{
    _pass = pass;
}

void User::setMessageCout(int cout)
{
    _messageCount = cout;
}

void User::addedMessage(int count)
{
    _messageCount = _messageCount + count;
}

void User::setCurrentID()
{
    _currentId++;
    _id = _currentId;
}

void User::setUserID(int id)
{
    _id = id;
}

void User::deletedMessage()
{
    if (_messageCount > 0)
    {
        _messageCount--;
    }
}

void User::saveUserPassword(const std::string &pass)
{
    _pass = pass;
}

std::istream &operator>>(std::istream &is, User &obj)
{
    std::string name;
    std::string login;
    std::string pass;
    long long id{0};
    bool isAdmin;
    bool isBanned;
    bool isDeleted;
    is >> id;
    is >> name;
    is >> login;
    is >> isAdmin;
    is >> isBanned;
    is >> isDeleted;
    int passStartPos = is.tellg();
    is.seekg(passStartPos + 1);
    std::getline(is, pass);
    obj.setUserName(name);
    obj.setUserLogin(login);
    obj.saveUserPassword(pass);
    obj.setUserID(id);
    obj.setIsAdmin(isAdmin);
    obj.setIsBanned(isBanned);
    return is;
}

std::ostream &operator<<(std::ostream &os, const User &obj)
{
    {
        os << obj.getId();
        os << " ";
        os << obj.getUserName();
        os << ' ';
        os << obj.getUserLogin();
        os << ' ';
        os << obj.isAdmin();
        os << ' ';
        os << obj.isBanned();
        os << ' ';
        os << obj.isDeleted();
        os << ' ';
        os << obj.getUserPassword();

        return os;
    }
}