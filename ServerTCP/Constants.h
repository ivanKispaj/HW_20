//
// Constants.h
// 2023-08-13
// Ivan Konishchev
//

#pragma once
#include <string>
#include "FileManager.h"

class Constants
{
    public:
    static struct _Constants
    {
        const std::string baseDIR{"./Desktop/ServerTCP"};
        const std::string dataDIR{"/data/"};
        const std::string configFileName{"config.txt"};
        const std::string messagesFile{"Messages.txt"};
        const std::string usersFile{"Users.txt"};

    } dir;
};
