
#include "ChatAdminInterface.h"

chat::Results ChatAdminInterface::run(std::unique_ptr<DB> _db)
{
    db = std::move(_db);
    auto complaintBotTemp = db->getUserByLogin("complaint_bot");
    this->complaintBot = std::move(complaintBotTemp);
    auto _login = login();
    if (_login == chat::login_success && user->isAdmin())
    {
        mainPage();
    }
    else
    {
        std::cout << "У вас не получилось войти в раздел администратора." << std::endl;
        return chat::login_cancel;
    }
    return chat::empty;
}

void ChatAdminInterface::mainPage()
{
    system(clear);
    pgDefault();
    UserInput<std::string, chat::Results> adminAreaPage(
        "Раздел администратора.",
        "Опции: "
        "\nп - управление пользователями;"
        "\nс - управление сообщениями;"
        "\nж - рассмотрение жалоб;"
        "\nв - выход;"
        "\nУкажите опцию: ",
        "Неверный ввод",
        4);

    adminAreaPage.addInputs("п", "с", "ж", "в");
    adminAreaPage.addOutputs(chat::user_list, chat::messages, chat::complaint, chat::back);

    do
    {
        system(clear);
        auto result = adminAreaPage.IOgetline();
        switch (result)
        {
        case chat::user_list:
            usersManage();
            break;
        case chat::messages:
            messagesManage();
            break;
        case chat::complaint:
            complaintManage();
            break;
        case chat::back:
            system(clear);
            pgDefault();
            return;
            break;
        default:
            break;
        }
    } while (1);
}

void ChatAdminInterface::complaintManage()
{
    system(clear);
    pgDefault();
    UserInput<std::string, chat::Results> complaintPage(std::string(), std::string(), "Неверный ввод", 5);
    complaintPage.addInputs("у", "нс", "п", "с", "н");
    complaintPage.addOutputs(chat::delete_message,
                             chat::chat_options,
                             chat::user_list,
                             chat::messages,
                             chat::back);

    do
    {
        std::string chatDescription;
        std::string chatMainMessage;
        system(clear);
        auto complaintList = db->getAllPrivateMessagesForUserById(complaintBot->getId(), pg_MaxItems);
        messagesList(std::move(complaintList));
        if (pg_MaxItems <= 0)
        {
            std::cout << "Жалобы отсутствуют." << std::endl;
        }
        chatDescription = "Список жалоб. Показаны жалобы: " + pgInfo();
        chatMainMessage = "Опции: " +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nу - удалить жалобу;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nнс - навигация по списку жалоб;") +
                          "\nп - управление пользователями;"
                          "\nс - управление сообщениями;"
                          "\nн - назад;"
                          "\nУкажите опцию: ";
        complaintPage.setDescription(chatDescription);
        complaintPage.setMainMessage(chatMainMessage);
        auto result = complaintPage.IOgetline();
        switch (result)
        {
        case chat::delete_message:
        {
            if (pg_MaxItems > 0)
            {
                deleteMessage(true);
            }
        }
        break;
        case chat::chat_options:
            pgNavigation();
            break;
        case chat::user_list:
            usersManage();
            break;
        case chat::messages:
            messagesManage();
            break;
        case chat::back:
            system(clear);
            pgDefault();
            return;
        default:
            break;
        }
    } while (1);
}

void ChatAdminInterface::messagesManage()
{
    system(clear);
    pgDefault();
    UserInput<std::string, chat::Results> messagesPage(std::string(), std::string(), "Неверный ввод", 3);
    messagesPage.addInputs("у", "нс", "н");
    messagesPage.addOutputs(chat::delete_message,
                            chat::chat_options,
                            chat::back);
    do
    {
        std::string chatDescription;
        std::string chatMainMessage;
        system(clear);
        auto messages = db->getAllPublicMessages(pg_MaxItems);
        messagesList(std::move(messages));
        if (pg_MaxItems <= 0)
        {
            std::cout << "Публичные сообщения отсутствуют." << std::endl;
        }
        chatDescription = "Список сообщений. Показаны сообщения: " + pgInfo();
        chatMainMessage = "Опции: " +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nу - удалить сообщение;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nнс - навигация по списку сообщений;") +
                          "\nн - назад;"
                          "\nУкажите опцию: ";
        messagesPage.setDescription(chatDescription);
        messagesPage.setMainMessage(chatMainMessage);
        auto result = messagesPage.IOgetline();
        switch (result)
        {
        case chat::delete_message:
        {
            if (pg_MaxItems > 0)
            {
                deleteMessage();
            }
        }
        break;
        case chat::chat_options:
            pgNavigation();
            break;
        case chat::back:
            system(clear);
            pgDefault();
            return;
        default:
            break;
        }
    } while (1);
}

void ChatAdminInterface::usersManage()
{
    system(clear);
    pgDefault();
    UserInput<std::string, chat::Results> usersPage(std::string(), std::string(), "Неверный ввод", 6);
    usersPage.addInputs("б", "р", "а", "на", "нс", "н");
    usersPage.addOutputs(chat::user_ban,
                         chat::user_unban,
                         chat::user_admin,
                         chat::user_not_admin,
                         chat::chat_options,
                         chat::back);

    do
    {
        std::string chatDescription;
        std::string chatMainMessage;
        system(clear);
        auto users = db->getAllUsers();
        pg_MaxItems = db->usersCount();
        usersList(std::move(users));

        if (pg_MaxItems <= 0)
        {
            std::cout << "В чате нет пользователей." << std::endl;
        }
        chatDescription = "Список пользователей. Показаны пользователи: " + pgInfo();
        chatMainMessage = "Опции: " +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nб - заблокировать;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nр - разблокировать;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nа - сделать администратором;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nна - снять администратора;") +
                          std::string((pg_MaxItems <= 0) ? std::string() : "\nнс - навигация по списку пользователей;") +
                          "\nн - назад;"
                          "\nУкажите опцию: ";
        usersPage.setDescription(chatDescription);
        usersPage.setMainMessage(chatMainMessage);
        auto result = usersPage.IOgetline();
        switch (result)
        {
        case chat::user_ban:
        {
            if (pg_MaxItems > 0)
            {
                userBan(true);
            }
        }
        break;
        case chat::user_unban:
        {
            if (pg_MaxItems > 0)
            {
                userBan(false);
            }
        }
        break;
        case chat::user_admin:
        {
            if (pg_MaxItems > 0)
            {
                userSetAdmin(true);
            }
        }
        break;
        case chat::user_not_admin:
        {
            if (pg_MaxItems > 0)
            {
                userSetAdmin(false);
            }
        }
        break;
        case chat::chat_options:
            pgNavigation();
            break;
        case chat::back:
            system(clear);
            pgDefault();
            return;
        default:
            break;
        }
    } while (1);
}

void ChatAdminInterface::deleteMessage(bool complaint_only)
{
    UserInput<int, int> getMessage(std::string(), "Укажите messageId: ", std::string());
    UserInput<std::string, chat::Results> yesnoIO(std::string(), "Вы действительно хотите удалить сообщение? (да - удалить / нет - не удалять): ", "Неверный ввод. Требуется да или нет", 4);
    yesnoIO.addInputs("да", "нет", "yes", "no");
    yesnoIO.addOutputs(chat::yes, chat::no, chat::yes, chat::no);

    do
    {
        int msgId = getMessage.IOcinThrough();
        auto message = db->getMessage(msgId);
        if (message != nullptr)
        {
            if (complaint_only && (message->getRecipientID() != 2))
            {
                std::cout << "Сообщение не является жалобой." << std::endl;
                continue;
            }
            if (!complaint_only && message->isPrivate)
            {
                std::cout << "Указан неверный messageId" << std::endl;
                continue;
            }
            auto result = yesnoIO.IOgetline();
            if (result == chat::yes)
            {
                db->deleteMessageById(msgId);
                return;
            }
            else
            {
                return;
            }
        }
        std::cout << "Указан неверный messageId" << std::endl;
    } while (true);
}

void ChatAdminInterface::userBan(bool ban)
{
    UserInput<int, int> getUser(std::string(), "Укажите userId: ", std::string());
    UserInput<std::string, chat::Results> yesnoIO(std::string(), std::string(), "Неверный ввод. Требуется да или нет", 4);
    yesnoIO.addInputs("да", "нет", "yes", "no");
    yesnoIO.addOutputs(chat::yes, chat::no, chat::yes, chat::no);

    do
    {
        int userId = 0;
        userId = getUser.IOcinThrough();
        auto _user = db->getUserById(userId);
        if (_user != nullptr)
        {
            if ((user->getId() == _user->getId()) || (_user->getId() == 2) || (_user->getId() == 1))
            {
                std::cout << "Операция недопустима для себя или сервисных админа или бота." << std::endl;
                yesnoIO.setMainMessage("Выйти из меню блокировки пользователей? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {
                    return;
                }
                continue;
            }
            if ((_user->isBanned() == ban) && ban)
            {
                std::cout << "Пользователь уже заблокирован." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {
                    return;
                }
                continue;
            }
            if ((_user->isBanned() == ban) && !ban)
            {
                std::cout << "Пользователь не заблокирован." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {
                    return;
                }
                continue;
            }
            if (ban)
            {
                yesnoIO.setMainMessage("Вы действительно хотите заблокировать пользователя? (да - заблокировать / нет - не блокировать): ");
            }
            else
            {
                yesnoIO.setMainMessage("Вы действительно хотите разблокировать пользователя? (да - разблокировать / нет - оставить заблокированным): ");
            }
            auto result = yesnoIO.IOgetline();
            if (result == chat::yes)
            {
                _user->setIsBanned(ban);
                if (ban)
                {
                    _user->setIsAdmin(false);
                }
                db->updateUserData(*_user);
                return;
            }
            else
            {
                return;
            }
        }
        std::cout << "Указан неверный userId" << std::endl;
    } while (true);
}

void ChatAdminInterface::userSetAdmin(bool adm)
{
    UserInput<int, int> getUser(std::string(), "Укажите userId: ", std::string());
    UserInput<std::string, chat::Results> yesnoIO(std::string(), std::string(), "Неверный ввод. Требуется да или нет", 4);
    yesnoIO.addInputs("да", "нет", "yes", "no");
    yesnoIO.addOutputs(chat::yes, chat::no, chat::yes, chat::no);
    auto result = chat::empty;
    do
    {
        int userId = 0;
        userId = getUser.IOcinThrough();
        auto _user = db->getUserById(userId);
        if (_user != nullptr)
        {
            if ((user->getId() == _user->getId()) || (_user->getId() == 2) || (_user->getId() == 1))
            {
                std::cout << "Нельзя менять статус администратора самого себя или сервисных админа или бота." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {

                    return;
                }
                continue;
            }
            if (_user->isBanned())
            {
                std::cout << "Этот пользователь заблокирован. Его нельзя сделать администратором." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {

                    return;
                }
                continue;
            }
            if ((_user->isAdmin() == adm) && adm)
            {
                std::cout << "Пользователь уже администратор." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {

                    return;
                }
                continue;
            }
            if ((_user->isAdmin() == adm) && !adm)
            {
                std::cout << "Пользователь не является администратором." << std::endl;
                yesnoIO.setMainMessage("Выйти? (да - выйти / нет - не выходить): ");
                if (yesnoIO.IOgetline() == chat::yes)
                {
                    return;
                }
                continue;
            }
            if (adm)
            {
                yesnoIO.setMainMessage("Вы действительно хотите дать права администратора? ( да / нет ): ");
            }
            else
            {
                yesnoIO.setMainMessage("Вы действительно хотите снять права администратора? ( да / нет ): ");
            }
            result = yesnoIO.IOgetline();
            if (result == chat::yes)
            {
                _user->setIsAdmin(adm);
                db->updateUserData(*_user);

                return;
            }
            else
            {
                return;
            }
        }
        std::cout << "Указан неверный userId" << std::endl;
    } while (true);
}