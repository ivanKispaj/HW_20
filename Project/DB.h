//
// DB.h
// 2023-03-11
// Ivan Konishchev
//

#pragma once

// #include "DBCoreArray.h"
#include "DBCoreMap.h"
#include "User.h"
#include "Message.h"
#include <memory>
#include <fstream>
#include <filesystem>
#include <stdlib.h>


class DB
{

    IDBCore<User> *_userDB = new DBCoreMap<User>();          // create table User
    IDBCore<Message> *_messageDB = new DBCoreMap<Message>(); // create table Message
    std::string _fileBaseDir = "./bin/";
    std::string _usersFile = "Users.txt";
    std::string _messagesFile = "Messages.txt";
public:
    /// @brief default init
    DB() = default;

    /// @brief Deinit  - clear memory
    ~DB()
    {
        delete _userDB;
        delete _messageDB;
    }

    void saveToFileData();
    void loadMessagesFromFile();
    bool loadUsersFromFile();

    /// @brief adds a new user to the database
    /// @param user struct User
    /// @return true - if is success, false if login non-unique!
    bool addUser(User &user);

    /// @brief adds a message to the database in a private chat
    /// @param message struct Message type
    void addMessage(Message &message);

    /// @brief adds a message to the database in a public chat, for all users
    /// @param message
    void AddMessageToAllUsers(Message &message);

    /// @brief Sets the user account password
    /// @param userID int unique id
    /// @param pass std::string new password
    /// @return true - if is success, false - if there is no user with this id
    bool setUserPassword(int userID, const std::string &pass);

    /// @brief Checks whether the login is unique.
    /// @param login std::string.
    /// @return true - if login unique, false - if the login exists.
    bool isUniqueLogin(const std::string &login);

    /// @brief checks whether passwords match or not
    /// @param userID int ID user
    /// @param pass std::string incoming password
    /// @return true - if the passwords match, false - if the passwords are different
    bool isCorrectPassword(int userID, const std::string &pass);

    /// @brief Returns an array of users
    /// @return  if the array is not empty - std::unique_ptr<User[]> , nullptr if the array is empty!
    const std::unique_ptr<User[]> getAllUsers() const;

    /// @brief Get a user by login
    /// @param login string -> user login
    /// @param exception
    /// @return Returning a pointer to a user object
    const std::unique_ptr<User> getUserByLogin(const std::string &login, bool exception = false) const;

    /// @brief Get a user by ID
    /// @param userId
    /// @return
    const std::unique_ptr<User> getUserById(const int &userId) const;

    /// @brief Returns the size of the user array
    /// @return  int user array size
    int usersCount() const;

    // /// @brief Returns an array of all messages
    // /// @return if the array is not empty - std::unique_ptr<Message[]>, nullptr if the array is empty!
    // const std::unique_ptr<Message[]> getAllMessages() const;

    const std::unique_ptr<Message> getMessage(const int &messageId) const;

    /// @brief Updates the user's data (name, login), except for the password!
    /// @param user struct User
    /// @return true if successful, false if not a unique login, no user, no users
    bool updateUserData(const User &user);

    /// @brief
    /// @param message
    /// @return
    bool deleteMessageById(int Id);

    /// @brief Returns an array of all incoming/outgoing Private messages between users (user1 and user2) in chronological ascending order
    /// @param user1Id int ID of the first user
    /// @param user2Id int ID of the second user
    /// @param size the reference is the size of the array, the created size of the array will be written to it
    /// @return std::unique_ptr<Message[]> Returns an array
    const std::unique_ptr<Message[]> getAllPrivateMessagesForUsersById(int user1Id, int user2Id, int &size) const;

    /// @brief Returns an array of all incoming/outgoing Private messages between user in chronological ascending order
    /// @param userId
    /// @param size the reference is the size of the array, the created size of the array will be written to it
    /// @return std::unique_ptr<Message[]> Returns an array
    const std::unique_ptr<Message[]> getAllPrivateMessagesForUserById(int userId, int &size) const;

    /// @brief Returns an array of all incoming/outgoing Public messages between users with ID in chronological ascending order
    /// @param Id int ID of the user
    /// @param size the reference is the size of the array, the created size of the array will be written to it
    /// @return std::unique_ptr<Message[]> Returns an array
    const std::unique_ptr<Message[]> getAllPublicMessagesForUserById(int Id, int &size) const;

    /// @brief Returns an array of all incoming/outgoing Public chronological ascending order
    /// @param size the reference is the size of the array, the created size of the array will be written to it
    /// @return std::unique_ptr<Message[]> Returns an array
    const std::unique_ptr<Message[]> getAllPublicMessages(int &size) const;
};