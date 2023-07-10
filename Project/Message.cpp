
//
// Message.cpp
// 2023-03-11
// Ivan Konishchev
//

#include "Message.h"
#include <chrono>
#include <memory>

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