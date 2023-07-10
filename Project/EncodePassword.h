
//
// EncodePassword.h
// 2023-03-11
// Ivan Konishchev
//
/*
    description :
    use -> #include "EncodePassword.h"
    std::string password = "kjasnfFKJFf983";
    EncodePassword::encodePassword(password);
    further along the code, the password variable will already be encoded
*/

#pragma once
#include <string>

class EncodePassword
{
public:
    /// @brief Password encoding, using a bit shift to the left with the addition of salt
    /// @param pass std::string password
    static void encodePassword(std::string &pass)
    {
        std::string _salt = "POSVoqe9)!3#kfafkqfKLNOEQIg09#G";

        std::string out;
        int index = 0;
        while (index < pass.size())
        {
            char ch = pass[index];
            char symbol = ch - pass[pass.size() - index] + _salt[index];
            out.push_back(symbol);
            out.push_back(_salt[index]);
            index++;
        }

        while (index < _salt.size())
        {
            char ch = _salt[index] >> 1;
            char symbol = ch + '!';
            out.push_back(symbol);
            index++;
        }

        pass = out;
    }
};
