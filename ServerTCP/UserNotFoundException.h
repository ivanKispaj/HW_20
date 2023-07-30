

#pragma once
#include <exception>

class UserNotFoundException : public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return "ERROR: пользователь не найден.";
    }
};