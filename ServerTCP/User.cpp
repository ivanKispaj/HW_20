
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

User::User(const std::string &name, const std::string &login, const std::string &pass) : _name(name), _login(login), _pass(pass)
{
    EncodePassword::encodePassword(_pass);
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
    _pass = pass;
    EncodePassword::encodePassword(_pass);
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

char *User::parseToBinaryData(int &size)
{
    size += sizeof(_id); // размер поля id
    size += sizeof(int); // размер длинны строки
    size += _name.length() * sizeof(_name[0]);
    size += sizeof(int);
    size += _login.length() * sizeof(_login[0]);
    size += sizeof(int);
    size += _pass.length() * sizeof(_pass[0]);
    size += sizeof(_isAdmin);
    size += sizeof(_isBanned);
    size += sizeof(_isDeleted);

    char* data = new char[size];
    int shift{0}, sizeString{0};

    memcpy(data, &_id, sizeof(_id));
    sizeString = _name.length();
    shift += sizeof(_id);
    memcpy(data + shift, &sizeString, sizeof(sizeString));
    shift += sizeof(sizeString);
    memcpy(data + shift, &_name[0], sizeString);
    shift += sizeString;
    sizeString = _login.length();
    memcpy(data + shift, &sizeString, sizeof(sizeString));
    shift += sizeof(sizeString);
    memcpy(data + shift, &_login[0], sizeString);
    shift += sizeString;
    sizeString = _pass.length();
    memcpy(data + shift, &sizeString, sizeof(sizeString));
    shift += sizeof(sizeString);
    memcpy(data + shift, &_pass[0], sizeString);
    shift += sizeString;
    memcpy(data + shift, &_isAdmin, sizeof(_isAdmin));
    shift += sizeof(_isAdmin);
    memcpy(data + shift, &_isBanned, sizeof(_isBanned));
    shift += sizeof(_isBanned);
    memcpy(data + shift, &_isDeleted, sizeof(_isDeleted));
    return data;

}

void User::parseFromBinaryData(char *data)
{
    int sizeString{0}, shift{0};
    memcpy(&_id, data, sizeof(_id));
    shift += sizeof(_id);
    memcpy(&sizeString, data + shift, sizeof(sizeString));
    shift += sizeof(sizeString);
    _name.resize(sizeString);
    memcpy(&_name[0], data + shift, sizeString);
    shift += sizeString;
    memcpy(&sizeString, data + shift, sizeof(sizeString));
    shift += sizeof(sizeString);
    _login.resize(sizeString);
    memcpy(&_login[0], data + shift, sizeString);
    shift += sizeString;
    memcpy(&sizeString, data + shift, sizeof(sizeString));
    shift += sizeof(sizeString);
    _pass.resize(sizeString);
    memcpy(&_pass[0], data + shift, sizeString);
    shift += sizeString;
    memcpy(&_isAdmin, data + shift, sizeof(_isAdmin));
    shift += sizeof(_isAdmin);
    memcpy(&_isBanned, data + shift, sizeof(_isBanned));
    shift += sizeof(_isBanned);
    memcpy(&_isDeleted, data + shift, sizeof(_isDeleted));
}