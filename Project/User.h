

// User.h
// 2023-03-11
// Ivan Konishchev
//

#pragma once
#include <string>
#include <fstream>

struct User
{
    friend class DB;

private:
    std::string _name;
    std::string _login;
    std::string _pass;
    int _messageCount{0};
    long long int _id{-1};
    static long long int _currentId;
    bool _isAdmin{false};   // true if the user is an admin
    bool _isBanned{false};  // true if the user is an banned
    bool _isDeleted{false}; // true if the user is an deleted account

    /// @brief encoding and set the user's password this is private method, the DB class has access to
    /// @param pass unencoded password
    void setUserPassword(const std::string &pass);

    /// @brief copies the already encoded password to the user property ( needed when the object is copied )
    ///        private method, the DB class has access to
    /// @param pass
    void copyUserPassword(const std::string &pass);

    /// @brief sets the number of messages the user has when copying it
    /// @param cout int
    void setMessageCout(int cout);

    /// @brief increases the message counter when adding messages to the database (DB)
    ///        private method, the DB class has access to
    void addedMessage(int count = 1);

    /// @brief Creates a user ID when creating a user
    ///        private method, the DB class has access to
    void setCurrentID();

    /// @brief sets the user ID when copying the user
    ///        private method, the DB class has access to
    /// @param id int
    void setUserID(int id);

    /// @brief deleted user count message
    void deletedMessage();

public:
    // default init without parameters
    User() = default;

    // default deinit without parameters
    ~User() = default;

    /// @brief init
    /// @param name  string user name
    /// @param login string user login unique!
    User(const std::string &name, const std::string &login) : _name(name), _login(login) {}

    /// @brief init
    /// @param name string name
    /// @param login string login unique!
    /// @param pass string password
    User(const std::string &name, const std::string &login, const std::string &pass);

    /// @brief  public method
    /// @return return the user's ID
    long long int getId() const;

    /// @brief public method
    /// @return  return the user's name
    const std::string &getUserName() const;

    /// @brief public method
    /// @return // return the user's login
    const std::string &getUserLogin() const;

    /// @brief public method
    /// @return return the user's password in encoded form
    const std::string &getUserPassword() const;

    /// @brief  set the user's name
    /// @param name string
    void setUserName(const std::string &name); // set the user's name

    /// @brief set the user's login
    /// @param login string
    void setUserLogin(const std::string &login); // set the user's login

    /// @brief To get the number of messages from the user - outgoing/incoming
    /// @return int outgoing/incoming count
    int getMessagesCount() const;

    /// @brief public method of checking whether the user is an administrator
    /// @return  return  true -> user is Admin, false -> user is not Admin
    bool isAdmin() const;

    /// @brief The method of checking whether the user is banned or not
    /// @return  true -> user as Banned, false -> user as not Banned
    bool isBanned() const;

    /// @brief The method of checking whether the user account is valid or not
    /// @return true -> user account is deleted
    bool isDeleted() const;

    /// @brief Sets the user as an administrator if true is passed
    /// @param isAdmin pass TRUE if the user is an admin, or it will FALSE
    void setIsAdmin(bool isAdmin);

    /// @brief Sets the status of the user banned if true is passed
    /// @param isBanned pass true if the user is an banned, or it will false
    void setIsBanned(bool isBanned);

    /// @brief Erases user data , except for ID and Login, or it will false
    void deleteThisData();

    /// @brief Overloading the assignment operator "="
    /// @param user User to assign struct "User"
    /// @return User
    User &operator=(const User &user);

    void saveUserPassword(const std::string &pass);

    friend std::istream &operator>>(std::istream &is, User &obj);
  
    friend std::ostream &operator<<(std::ostream &os, const User &obj);
 
};
