
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
#include <sstream>

typedef unsigned int uint;

class EncodePassword
{
    static const int one_block_size_bytes{64};
    static const int one_block_size_uints{16};
    static const int block_expend_size_uints{80};

    static const int SHA1HASHLENGTHBYTES{20};
    static const int SHA1HASHLENGTHUINTS{5};

    typedef uint *Block;
    typedef uint ExpendBlock[80];

    EncodePassword();

    static uint cycle_shift_left(uint val, int bit_count);
    static uint bring_to_human_view(uint val);
public:
    static void sha1(std::string &value);
};

