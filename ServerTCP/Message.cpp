
//
// Message.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "Message.h"

long long int Message::_currentId;

void Message::setMessage(const std::string &message)
{
    _message = message;
}

void Message::setAuthorID(int authorID)
{
    _authorID = authorID;
}

void Message::setRecipientID(int recipientID)
{
    _recipientID = recipientID;
}

void Message::setId(long long int id)
{
    _id = id;
}
const std::string &Message::getMessage() const
{
    return _message;
}

int Message::getAuthorID() const
{
    return _authorID;
}

int Message::getRecipientID() const
{
    return _recipientID;
}

long long int Message::getId() const
{
    return _id;
}

long long int Message::getDate() const
{
    return _date;
}

Message &Message::operator=(const Message &message)
{
    isPrivate = message.isPrivate;
    _message = message.getMessage();
    _authorID = message.getAuthorID();
    _recipientID = message.getRecipientID();
    _date = message.getDate();
    _id = message.getId();
    return *this;
}

// Private methods for Friends "DB"
void Message::setDateMessage()
{
    using namespace std::chrono;
    long long int sec = int(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
    _date = sec;
}

void Message::setDateMessage(int date)
{
    _date = date;
}
void Message::setMessageId()
{
    _currentId++;
    _id = _currentId;
}

void Message::deleteThisData()
{
}

std::istream &operator>>(std::istream &is, Message &obj)
{
    std::string message;
    int authorID{0};
    int recipientID{0};
    long long int date{0};
    int id{0};
    is >> id;
    is >> authorID;
    is >> recipientID;
    is >> date;
    is >> obj.isPrivate;
    std::getline(is, message);

    obj.setId(id);
    obj.setMessage(message);
    obj.setAuthorID(authorID);
    obj.setRecipientID(recipientID);
    return is;
}

std::ostream &operator<<(std::ostream &os, const Message &obj)
{
    os << obj.getId();
    os << ' ';
    os << obj.getAuthorID();
    os << ' ';
    os << obj.getRecipientID();
    os << ' ';
    os << obj.getDate();
    os << ' ';
    os << obj.isPrivate;
    os << ' ';
    os << obj.getMessage();
    return os;
}

std::vector<std::string> Message::getQueryValuesForInsert()
{
    std::vector<std::string> ret;
    std::string value{"'"};
    value.append(_message);
    value.append("','");
    value.append(isPrivate ? "1" : "0");
    value.append("','");
    value.append(std::to_string(_authorID));
    value.append("','");
    value.append(std::to_string(_recipientID));
    value.append("',");
    ret.push_back(value);
    return ret;
}

char *Message::parseToBinaryData(int &size)
{
    size += sizeof(isPrivate);
    size += sizeof(int);
    size += _message.length() * sizeof(_message[0]);
    size += sizeof(_authorID);
    size += sizeof(_recipientID);
    size += sizeof(_date);
    size += sizeof(_id);

    char *data = new char[size];
    int shift{0}, sizeString{0};
    memcpy(data, &isPrivate, sizeof(isPrivate));
    shift += sizeof(isPrivate);
    sizeString = _message.length();
    memcpy(data + shift, &sizeString, sizeof(sizeString));
    shift += sizeof(sizeString);
    memcpy(data + shift, &_message[0], sizeString);
    shift += sizeString;
    memcpy(data + shift, &_authorID, sizeof(_authorID));
    shift += sizeof(_authorID);
    memcpy(data + shift, &_recipientID, sizeof(_recipientID));
    shift += sizeof(_recipientID);
    memcpy(data + shift, &_date, sizeof(_date));
    shift += sizeof(_date);
    memcpy(data + shift, &_id, sizeof(_id));
    return data;
}

void Message::parseFromBinaryData(char *data)
{
    int sizeString{0}, shift{0};
    memcpy(&isPrivate, data, sizeof(isPrivate));
    shift += sizeof(isPrivate);
    memcpy(&sizeString, data + shift, sizeof(sizeString));
    shift += sizeof(sizeString);
    _message.resize(sizeString);
    memcpy(&_message[0], data + shift, sizeString);
    shift += sizeString;
    memcpy(&_authorID, data + shift, sizeof(_authorID));
    shift += sizeof(_authorID);
    memcpy(&_recipientID, data + shift, sizeof(_recipientID));
    shift += sizeof(_recipientID);
    memcpy(&_date, data + shift, sizeof(_date));
    shift += sizeof(_date);
    memcpy(&_id, data + shift, sizeof(_id));
}

void Message::parseFromMysqlData(MYSQL_ROW data)
{
    _id = std::atoi(data[0]);
    _message = std::string(data[1]);
    isPrivate = (*data[2] == '1');
    _authorID = std::atoi(data[3]);
    _recipientID = std::atoi(data[4]);
    _date = std::atoi(data[5]);
}

std::vector<std::string> Message::getQueryValueForUpdate()
{
    // Доработать на будущий функционал!
    std::vector<std::string> str;
    return str;
}