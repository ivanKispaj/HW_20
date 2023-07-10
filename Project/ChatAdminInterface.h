#pragma once
#include "IChatInterface.h"
class ChatAdminInterface final : public IChatInterface
{
private:
    std::unique_ptr<User> complaintBot = nullptr;

public:
    ChatAdminInterface() = default;
    ~ChatAdminInterface() = default;
    chat::Results run(std::unique_ptr<DB> _db) override;

    /// @brief отображает главную страницу раздела администратора
    void mainPage();

    /// @brief Страница обработки жалоб
    void complaintManage();

    /// @brief страница работы с сообщениями общего чата
    void messagesManage();

    /// @brief страница работы с пользователями
    void usersManage();

    /// @brief метод удаления сообщений
    /// @param complaint_only - true если надо удалять жалобы сервисного бота
    void deleteMessage(bool complaint_only = false);

    /// @brief Блокировка пользователя
    /// @param ban true - для блокирования; false - для разблокировки
    void userBan(bool ban = true);

    /// @brief Изменяет статус администратора
    /// @param adm true - сделать администратором; false - снять с должности администратора
    void userSetAdmin(bool adm = true);
};