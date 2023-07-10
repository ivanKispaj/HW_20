//
// IDBCore.h
// 2023-05-28
// Ivan Konishchev
//

#pragma once
#include <memory>

template <typename T>
class IDBCore
{
public:
    virtual void append(T &value) = 0;
    virtual int count() const = 0;
    virtual bool isEmpty() const = 0;
    virtual void deleteById(int id) = 0;
    virtual std::unique_ptr<T[]> getTableData(int &size) const = 0;
    virtual bool updateData(T &data) = 0;
    virtual std::unique_ptr<T> getDataById(int id) const = 0;
    virtual IDBCore<T> &operator--(int ID) = 0;
    virtual ~IDBCore() = default;
};
