//
// Message.h
// 2023-03-11
// Ivan Konishchev
//

#pragma once
#include <string>
#include <fstream>

struct Message
{

    friend class DB;

    // true - a message from a private chat
    // false = true - a message from a public chat
    bool isPrivate{false};

    /// @brief default init
    Message() = default;

    /// @brief default deinit
    ~Message() = default;

    /// @brief init
    /// @param message string message
    /// @param authorID int author ID
    /// @param recipientID int recipient ID
    /// @param privateChat bool -> true - private chat / false - public chat
    Message(const std::string &message, int authorID, int recipientID, bool privateChat = false) : isPrivate(privateChat),
                                                                                                   _message(message),
                                                                                                   _authorID(authorID),
                                                                                                   _recipientID(recipientID) {}

    /// @brief Assigns a message to an object
    /// @param message string
    void setMessage(const std::string &message);

    /// @brief Assigns a author ID to an object
    /// @param authorID int -> ID of the author of the message
    void setAuthorID(int authorID);

    /// @brief Assigns a recipient ID to an object
    /// @param recipientID int -> ID of the message recipient
    void setRecipientID(int recipientID);

    void setId(long long int id);

    /// @brief Returns a message as a string
    /// @return string
    const std::string &getMessage() const;

    /// @brief Returns the ID of the author of the message
    /// @return int author ID
    int getAuthorID() const;

    /// @brief Returns the ID of the recipient of the message
    /// @return int recipient ID
    int getRecipientID() const;

    /// @brief Returns the ID of the message
    /// @return int
    long long int getId() const;

    /// @brief Unix Timestamp returns the date in seconds from 1970
    /// @return long long int -> timestamp date in seconds
    long long int getDate() const;

    // #undef private
    //     // Private methods for Friends "DB"

    /// @brief set date Unix Timestamp
    void setDateMessage(int date);
    // /// @brief  set message unique id
    // void setMessageId();

    //     /// @brief Overloading the assignment operator "="
    //     /// @param message Message to assign struct "Message"
    //     /// @return Message
    //     Message &operator=(const Message &message);
    /// @brief Overloading the assignment operator "="
    /// @param message Message to assign struct "Message"
    /// @return Message
    Message &operator=(const Message &message);
    void deleteThisData();

    friend std::ostream &operator<<(std::ostream &os, const Message &obj);
    friend std::istream &operator>>(std::istream &is, Message &obj);
private:
    std::string _message;
    int _authorID = 0;
    int _recipientID = 0;
    long long int _date{0};
    int _id{0};
    static long long int _currentId;
    // Private methods for Friends "DB"

    /// @brief set date Unix Timestamp
    void setDateMessage();

    /// @brief  set message unique id
    void setMessageId();
};