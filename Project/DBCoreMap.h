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

template <typename T>
class DBCoreMap : public IDBCore<T>
{
public:
    enum TableName
    {
        users,
        messages
    };
    // Init default
    DBCoreMap<T>() = default;

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

    void append(T &value) override
    {
        _table[value.getId()] = value;
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

private:
    std::map<int, T> _table;
};