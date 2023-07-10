#pragma once
#include "IChatInterface.h"
#include <iostream>

class ChatUserInterface final : public IChatInterface
{
private:
    std::unique_ptr<User> pm_user = nullptr;        // участник переписки в личных сообщениях
    std::unique_ptr<User[]> pm_user_list = nullptr; // список пользователей для приватного чата
public:
    ChatUserInterface() = default;
    ~ChatUserInterface() = default;

    /// @brief Первоначальный запуск чата
    /// @return chat::Results
    chat::Results run(std::unique_ptr<DB> _db) override;

    /// @brief Интерактивное меню входа в общий чат.
    /// @return chat::Results
    chat::Results loginInChat();

    /// @brief Интерактивное меню регистрации пользователя. При успешной регистрации - автоматический вход в чат.
    /// @return chat::Results
    chat::Results registration();

    /// @brief Интерактивное меню общего чата.
    /// @return
    chat::Results publicChat();

    /// @brief Посылает сообщение в общий чат
    void sendMessage(chat::Results result = chat::public_chat);

    /// @brief Интерактивное меню личных сообщений. Главная страница. Выбор пользователя для беседы.
    /// @return
    chat::Results privateChat();

    /// @brief Поиск пользователя
    /// @param result задает критерии: по логину или имени
    void searchUser(chat::Results result);
    chat::Results privateChatWithUser(chat::Results result);

    /// @brief Интерактивное меню жалоб на сообщения
    void complaint();
};