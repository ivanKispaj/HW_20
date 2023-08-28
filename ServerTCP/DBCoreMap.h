//
// DBCoreMap.h
// 2023-05-31
// Ivan Konishchev
//

#pragma once

#include "IDBCore.h"
#include <memory>
#include <utility>
#include <stdexcept>
#include "UserNotFoundException.h"
#include <map>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "FileManager.h"
#include "Constants.h"
#include <typeinfo>
#include "Message.h"
#include "User.h"

template <typename T>
class DBCoreMap : public IDBCore<T>
{
public:
    // Init default
    DBCoreMap<T>() = delete;

    DBCoreMap<T>(const std::string &tName) : _tName(tName) {}

    // deinit default
    ~DBCoreMap() = default;

    // Copy construction
    DBCoreMap<T>(const DBCoreMap &value)
    {
        for (auto const &[key, data] : value._table)
        {
            _table[key] = data;
        }
    }

    bool append(T &value) override
    {
        _table[value.getId()] = value;
        return true;
    }

    // return size of array
    int count() const override
    {
        return _table.size();
    }

    // return true if array is empty
    bool isEmpty() const override
    {
        return _table.empty();
    }

    IDBCore<T> &operator--(int ID) override
    {
        if (_table.size() > 0)
        {
            auto it = _table.find(ID);
            if (it != _table.end())
            {
                _table.erase(it);
            }
        }
        return *this;
    }

    void deleteById(int Id) override
    {
        if (_table.size() > 0)
        {
            auto it = _table.find(Id);
            if (it != _table.end())
            {
                _table.erase(it);
            }
        }
    }

    std::unique_ptr<T[]> getTableData(int &size) const override
    {
        std::unique_ptr<T[]> ret(new T[_table.size()]);
        int index = 0;
        for (auto const &it : _table)
        {
            ret[index] = it.second;
            index++;
        }
        size = index;
        return ret;
    }

    bool updateData(T &data) override
    {
        for (auto const &it : _table)
        {
            if (it.second.getId() == data.getId())
            {
                _table[it.first] = data;
            }
        }
        return false;
    }

    std::unique_ptr<T> getDataById(int id) const override
    {
        auto it = _table.find(id);
        if (it != _table.end())
        {
            std::unique_ptr<T> ret(new T());
            (*ret) = it->second;
            return ret;
        }
        return nullptr;
    }
    bool isUnique(std::string &login) override
    {
        for (auto it = _table.begin(); it != _table.end(); it++)
        {
            if (it->second.getUserLogin() == login)
            {
                return false;
            }
        }
        return true;
    }

    // void createDefaultData() override
    // {
    //     //  std::string file = Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.usersFile

    //     if (!loadFromFileData())
    //     {
    //         if (_tName == "users")
    //         {
    //             std::cout << "Create admin and complain_bot users!\n";
    //             T service_admin("admin", "admin", "1234");
    //             service_admin.setIsAdmin(true);
    //             append(service_admin);
    //             T service_bot("complaint_bot", "complaint_bot", "uijyh6758r9ifSDFG4rdfcvgtre"); // Service Admin userID = 1;
    //             append(service_bot);
    //             std::cout << "Логин сервисного администратора: admin, пароль: 1234" << std::endl;
    //             std::cout << "Рекомендуем сменить пароль. Войдите в чат -> настройки профиля." << std::endl;
    //             std::cout << "Это сообщение больше не будет отображаться." << std::endl;
    //             std::cout << std::endl;
    //             std::cout << "Для остановки сервера введите -> 'q'\n";
    //             std::cout << std::endl;
    //             saveToFileData();
    //         }
    //     }
    // }

    void saveToFileData() override
    {
        std::string saveFile;
        if (_tName == "users")
        {
            saveFile = Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.usersFile;
        }
        else if (_tName == "messages")
        {
            saveFile = Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.messagesFile;
        }
        else
        {
            return;
        }

        if (!_table.empty())
        {
            int count = 0;
            std::unique_ptr<T[]> data = getTableData(count);

            FileManager::File file(saveFile, FileManager::StreamType::save, true, true);

            for (int i = 0; i < count; i++)
            {
                file.saveData(data[i]);
            }
        }
    }

    bool loadFromFileData() override
    {
        std::string loadFile;
        if (_tName == "users")
        {
            loadFile = Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.usersFile;
        }
        else if (_tName == "messages")
        {
            loadFile = Constants::dir.baseDIR + Constants::dir.dataDIR + Constants::dir.messagesFile;
        }
        else
        {
            return false;
        }
        FileManager::File file(loadFile, FileManager::StreamType::save, true, true);
        T data;
        while (file.loadData(data))
        {
            data._currentId++;
            append(data);
        }
        if (_table.empty())
        {
            return false;
        }
        return true;
    }

private:
    std::map<int, T> _table;
    std::string _tName;
};