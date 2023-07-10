
//
// DBCoreArray.h
// 2023-03-11
// Ivan Konishchev
//

#pragma once
#include "IDBCore.h"
#include <memory>
#include <utility>
#include <stdexcept>
#include "UserNotFoundException.h"
#include <iostream>

template <typename T>
class DBCoreArray : public IDBCore<T>
{
public:
    enum TableName
    {
        users,
        messages
    };
    // Init default
    DBCoreArray<T>() : _size(0), _array(nullptr){};

    // deinit default
    ~DBCoreArray() = default;

    // Copy construction
    DBCoreArray<T>(const DBCoreArray &array) : _size(array._size), _array(std::make_unique<T[]>(array._size))
    {

        for (int i = 0; i < _size; i++)
        {
            _array[i] = array._array[i];
        }
    }

    void append(T &value) override
    {
        _size++;
        std::unique_ptr<T[]> newArray = std::make_unique<T[]>(_size);
        if (_size <= 1)
        {
            newArray[0] = value;
        }
        else
        {
            for (int i = 0; i < _size - 1; i++)
            {
                newArray[i] = _array[i];
            }
            newArray[_size - 1] = value;
        }

        _array = std::move(newArray);
    }

    // return size of array
    int count() const override
    {
        return _size;
    }

    // return true if array is empty
    bool isEmpty() const override
    {
        return _size == 0;
    }

    IDBCore<T> &operator--(int ID) override
    {
        if (_size > 0)
        {
            for (int i = 0; i < _size; i++)
            {
                if (_array[i].getId() == ID)
                {
                    _array[i].deleteThisData();
                }
            }
        }
        return *this;
    }

    void deleteById(int Id) override
    {
        if (_size > 0)
        {
            std::unique_ptr<T[]> temporaryArray = std::make_unique<T[]>(_size);
            int size = 0;
            for (int i = 0; i < _size; i++)
            {
                if (_array[i].getId() != Id)
                {
                    temporaryArray[size] = _array[i];
                    size++;
                }
            }
            _array = nullptr;
            _array = std::make_unique<T[]>(size);
            for (int i = 0; i < size; i++)
            {
                _array[i] = temporaryArray[i];
            }
            _size = size;
        }
    }

    std::unique_ptr<T[]> getTableData(int &size) const override
    {
        std::unique_ptr<T[]> ret(new T[_size]);
        for (int i = 0; i < _size; i++)
        {
            ret[i] = _array[i];
        }
        size = _size;
        return ret;
    }

    bool updateData(T &data) override
    {
        for (int i = 0; i < _size; i++)
        {
            if (_array[i].getId() == data.getId())
            {
                _array[i] = data;
                return true;
            }
        }
        return false;
    }
    std::unique_ptr<T> getDataById(int id) const override
    {
        for (int i = 0; i < _size; i++)
        {
            if (_array[i].getId() == id)
            {
                std::unique_ptr<T> ret(new T());
                (*ret) = _array[i];
                return ret;
            }
        }
        return nullptr;
    }

private:
    int _size{0}; // size of array
    std::unique_ptr<T[]> _array = nullptr;
};