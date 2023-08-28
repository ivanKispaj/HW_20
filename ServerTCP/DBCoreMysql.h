//
// DBCoreMysql.h
// 2023-08-10
// Ivan Konishchev
//

#pragma once

#include "mysql.h"
#include "IDBCore.h"
#include <string>
#include <typeinfo>
#include "User.h"
#include "Message.h"
#include "Constants.h"

template <typename T>
class DBCoreMysql : public IDBCore<T>
{

    mutable MYSQL *_mysql;
    std::string _tableName;
    std::string _dataBaseName;
    std::string _host;
    std::string _user;
    std::string _pass;

    void connectToDB() const
    {
        _mysql = mysql_init(NULL);

        if (_mysql == NULL)
        {
            // Если дескриптор не получен — выводим сообщение об ошибке
            std::cout << "Error: can't create MySQL-descriptor" << std::endl;
            return;
        }
        // Подключаемся к серверу
        if (!mysql_real_connect(_mysql, _host.c_str(), _user.c_str(), _pass.c_str(), _dataBaseName.c_str(), 0, NULL, 0))
        {
            // Если нет возможности установить соединение с БД выводим сообщение об ошибке
            std::cout << "Error: can't connect to database " << mysql_error(_mysql) << std::endl;
            return;
        }
    }

    void closeConnection() const
    {
        if (_mysql != NULL)
        {
            std::cout << "Close connection the DB.\n";
            mysql_close(_mysql);
        }
    }

public:
    DBCoreMysql<T>() = delete;
    DBCoreMysql<T>(const std::string &tName)
    {
        std::string filePath = (Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.configFileName);
        FileManager::File file(filePath, FileManager::load, false, false);

        file.readStringInsideDelimiters('"', _host);
        file.readStringInsideDelimiters('"', _dataBaseName);
        file.readStringInsideDelimiters('"', _user);
        file.readStringInsideDelimiters('"', _pass);
        _tableName = tName;
        createDefaultTable();
    }

    ~DBCoreMysql()
    {
        closeConnection();
    }

    bool append(T &value) override
    {

        connectToDB();
        bool isSaccess{true};
        std::vector<std::string> data = value.getQueryValuesForInsert();
        std::string query;
        
        if (_tableName == "users")
        {
            MYSQL_RES *res;
            int status{-1};
            mysql_set_server_option(_mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);
            std::string query;
            query = "START TRANSACTION;";
            query.append("INSERT INTO " + std::string(_dataBaseName) + "." + std::string(_tableName) + " (name, login, isAdmin, isDeleted, isBanned)" + " VALUES(");
            query.append(data[0]);
            query.append(");");
            query.append("SET @a = LAST_INSERT_ID();");
            query.append(("INSERT INTO " + std::string(_dataBaseName) + "." + "users_pass (users_id, user_pass) VALUES (@a, \""));
            query.append(data[1]);
            query.append("\");");
            query.append(("COMMIT;"));
            status = mysql_query(_mysql, query.c_str());
            if (status)
            {
                std::cout << "Error mysql append! Could not execute statement(s)\n";
                mysql_close(_mysql);
                return false;
            }
            do
            {
                res = mysql_store_result(_mysql);
                if (res != NULL)
                {
                    mysql_free_result(res);
                }
                if ((status = mysql_next_result(_mysql)) > 0)
                {
                    std::cerr << "Error!\nCould not execute statement(s)\n";
                    isSaccess = false;
                }

            } while (status == 0);

            mysql_set_server_option(_mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
        }
        else if (_tableName == "messages")
        {
            query = "INSERT INTO " +
                    std::string(_dataBaseName) + "." + std::string(_tableName) +
                    "(message, isPrivate, author_id, recipient_id, `date`) VALUES(";
            query.append(data[0]);
            query.append("NOW());");

            if (mysql_query(_mysql, query.c_str()))
            {
                std::cout << "Error: mysql_query: " << mysql_error(_mysql) << std::endl;
                isSaccess = false;
            }
        }
        mysql_close(_mysql);
        return isSaccess;
    }

    int count() const override
    {
        connectToDB();
        std::string query{"SELECT COUNT(*) FROM " + _tableName};
        mysql_query(_mysql, query.c_str());
        MYSQL_RES *res;
        MYSQL_ROW row;
        int count{0};
        if ((res = mysql_store_result(_mysql)))
        {
            if ((row = mysql_fetch_row(res)))
            {
                count = std::atoi(row[0]);
            }
        }
        if (res)
        {
            mysql_free_result(res);
        }
        mysql_close(_mysql);
        return count;
    }

    bool isEmpty() const override
    {
        return (count() == 0);
    }

    void deleteById(int id) override
    {

        connectToDB();
      //  MYSQL_RES *res;
        std::string query{"DELETE FROM " + _tableName + " WHERE id = " + std::to_string(id) + ";"};
        mysql_query(_mysql, query.c_str());
        mysql_store_result(_mysql);
        // if (res)
        // {
        //     mysql_free_result(res);
        // }
        mysql_close(_mysql);
    }

    std::unique_ptr<T[]> getTableData(int &size) const override
    {

        if ((size = count()) > 0)
        {
            connectToDB();

            unsigned int index{0}, error{0};
            std::unique_ptr<T[]> values(new T[size]);

            // SELECT * FROM users u INNER JOIN users_pass up ON u.id = up.id WHERE u.id  = 2;
            std::string query{"SELECT * FROM " + _tableName + ""};

            if (_tableName == "users")
            {
                query += " u INNER JOIN users_pass up ON u.id = up.users_id";
            }
            query += ";";

            error = mysql_query(_mysql, query.c_str());
            if (error > 0)
            {
                std::cout << "error Mysql: " << mysql_error(_mysql) << "\n";
            }

            MYSQL_RES *res;
            MYSQL_ROW row;

            if ((res = mysql_store_result(_mysql)))
            {
                while ((row = mysql_fetch_row(res)))
                {

                    T value;
                    value.parseFromMysqlData(row);
                    values[index] = value;
                    index++;
                }
            }
            if (res != NULL)
            {
                mysql_free_result(res);
            }
            mysql_close(_mysql);
            return values;
        }
        return nullptr;
    }

    bool updateData(T &data) override
    {
        connectToDB();

        std::vector<std::string> value = data.getQueryValueForUpdate();
        std::string query("UPDATE " + _tableName + " SET ");
        query += value[0];
        if (_tableName == "users")
        {
            query += " WHERE login='" + value[1] + "';";

            if (!(mysql_query(_mysql, query.c_str())))
            {
                query = "UPDATE users_pass SET " + value[2] + ";";
                if (!(mysql_query(_mysql, query.c_str())))
                {
                    mysql_close(_mysql);
                    return true;
                }
            }
        }
        else
        {
            query += " WHERE id = '" + value[1] + "';";
            if (!(mysql_query(_mysql, query.c_str())))
            {
                mysql_close(_mysql);
                return true;
            }
        }
        mysql_close(_mysql);
        return false;
    }

    std::unique_ptr<T> getDataById(int id) const override
    {
        std::unique_ptr<T> user = std::make_unique<T>(T());
        return user;
    }

    IDBCore<T> &operator--(int ID) override
    {

        connectToDB();

        std::string query{"DELETE FROM " + _tableName + " WHERE id=" + std::to_string(ID) + ";"};
        mysql_query(_mysql, query.c_str());
        mysql_close(_mysql);
        return *this;
    }

    bool isUnique(std::string &login) override
    {

        connectToDB();

        std::string query{"SELECT EXISTS (SELECT * FROM " + _tableName + " u WHERE u.login  = '" + login + "');"};
        mysql_query(_mysql, query.c_str());
        MYSQL_RES *res;
        MYSQL_ROW row;
        if ((res = mysql_store_result(_mysql)))
        {
            if ((row = mysql_fetch_row(res)))
            {
                bool ret = *row[0] == '0';
                mysql_free_result(res);
                mysql_close(_mysql);
                return ret;
            }
        }
        if (res != NULL)
        {
            mysql_free_result(res);
        }
        mysql_close(_mysql);
        return false;
    }

    void createDefaultTable()
    {
        connectToDB();
        std::string query{"CREATE TABLE IF NOT EXISTS users (\
        id int NOT NULL AUTO_INCREMENT,\
        name varchar(50) NOT NULL, \
        login varchar(50) NOT NULL,\
        isAdmin tinyint(1) DEFAULT NULL,\
        isDeleted tinyint(1) DEFAULT NULL,\
        isBanned tinyint(1) DEFAULT NULL,\
        PRIMARY KEY (id),\
        UNIQUE KEY login (login)\
         ) ENGINE=InnoDB"};

        mysql_query(_mysql, query.c_str());
        query = "CREATE TABLE IF NOT EXISTS users_pass (\
                    users_id int DEFAULT NULL,\
                    user_pass varchar(255) DEFAULT NULL,\
                    KEY users_id (users_id),\
                    CONSTRAINT users_pass_ibfk_1 FOREIGN KEY (users_id) REFERENCES users (id) ON DELETE CASCADE\
                    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;";
        mysql_query(_mysql, query.c_str());

        query = "CREATE TABLE IF NOT EXISTS messages (\
                id int NOT NULL AUTO_INCREMENT,\
                message varchar(255) DEFAULT NULL,\
                isPrivate tinyint(1) DEFAULT NULL,\
                author_id int NOT NULL,\
                recipient_id int DEFAULT NULL,\
                date timestamp NOT NULL,\
                PRIMARY KEY (id),\
                CONSTRAINT `messages_ibfk_1` FOREIGN KEY (`author_id`) REFERENCES `users` (`id`)\
                ) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;";
        mysql_query(_mysql, query.c_str());
        mysql_close(_mysql);
    }
};
