//
// FileManager.h
// 2023-08-12
// Ivan Konishchev
//

#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace FileManager
{
    enum StreamType
    {
        load,
        save
    };

    class File
    {
        std::string _fileName;
        bool _isPrivateFile;
        std::fstream _stream;
        bool _fileIsOpen = false;

        void accessAllow()
        {
            std::filesystem::path path{_fileName};
            if (std::filesystem::exists(path))
            {
                std::filesystem::permissions(_fileName, std::filesystem::perms::owner_write,
                                             std::filesystem::perm_options::add);
            }
        }
        void accessDeny()
        {
            std::filesystem::path path{_fileName};
            if (std::filesystem::exists(path))
            {
                std::filesystem::permissions(_fileName, std::filesystem::perms::owner_all | std::filesystem::perms::group_all | std::filesystem::perms::others_all,
                                             std::filesystem::perm_options::remove);
            }
        }

    public:
        File(const std::string &fileName, const FileManager::StreamType &type, bool isNewFile = true, bool isPrivateFile = false) : _fileName(fileName), _isPrivateFile(isPrivateFile)
        {
            if (_isPrivateFile)
            {
                accessAllow();
            }

            switch (type)
            {
            case load:
                _stream.open(_fileName, std::ios::in);
                break;
            case save:
                if (isNewFile)
                {
                    _stream.open(_fileName, std::ios::out | std::ios::trunc);
                }
                else
                {
                    _stream.open(_fileName, std::ios::out | std::ios::app);
                }
                break;
            }

            if (_stream.is_open())
            {
                _fileIsOpen = true;
            }
            else
            {
                _fileIsOpen = false;
            }
        };

        File(const File &file) = delete;

        ~File()
        {
            if (_isPrivateFile)
            {
                accessDeny();
            }

            if (_stream.is_open())
            {
                _stream.close();
            }
            _fileIsOpen = false;
        }

        template <typename Value>
        void saveData(const Value &value)
        {
            if (_fileIsOpen)
            {
                _stream << value << "\n";
            }
            else
            {
                std::cout << "The file is not open!\n";
            }
        }

        template <typename Value>
        bool loadData(Value &value)
        {
            bool res = false;
            if (_fileIsOpen)
            {
                if (_stream >> value)
                {
                    res = true;
                }
            }
            else
            {
                std::cout << "The file is not open!\n";
            }
            return res;
        }

        std::string readLine(int numberLine)
        {

            if (_fileIsOpen)
            {
                _stream.seekg(0, std::ios::beg);
                int currentLine{1};
                std::string res;
                while (currentLine < numberLine)
                {
                    std::getline(_stream, res);
                    currentLine++;
                }
                std::getline(_stream, res);
                return res;
            }
            else
            {
                std::cout << "The file is not open!\n";
            }
            return "";
        }

        void readStringInsideDelimiters(char delimiter, std::string &str)
        {
            if (_fileIsOpen)
            {
                std::string data;
                std::getline(_stream, data, delimiter);
                std::getline(_stream, data, delimiter);
                str.append(data);
            }
            else
            {
                std::cout << "The file is not open!\n";
            }
        }
    };
}
