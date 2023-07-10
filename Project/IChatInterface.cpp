
#include "IChatInterface.h"

chat::Results IChatInterface::login()
{
    chat::Results endInput = chat::empty;
    std::string incorrectInput = "Неверный ввод. Пустые значения недопустимы.";

    UserInput<std::string, std::string> getLogin("Страница входа", "Введите логин: ", incorrectInput);
    UserInput<std::string, std::string> getPass(std::string(), "Введите пароль: ", incorrectInput);
    UserInput<std::string, chat::Results> loginCancel(std::string(), "Отменить вход? (да - отменить, нет - продолжить): ", "Неверный ввод. Требуется: да или нет", 4);
    loginCancel.addInputs("да", "нет", "yes", "no");
    loginCancel.addOutputs(chat::yes, chat::no, chat::yes, chat::no);

    // ввод логина
    do
    {
        std::string _login = getLogin.IOgetlineThrough(true);

        try
        {
            std::unique_ptr<User> _user = db->getUserByLogin(_login, true);
            user = std::move(_user);
            break;
        }
        catch (UserNotFoundException &e)
        {
            std::cout << e.what() << std::endl;
            endInput = loginCancel.IOgetline();
            if (endInput == chat::yes)
            {
                return chat::login_cancel;
            }
        }
    } while (1);

    // ввод пароля
    bool validate = false;
    do
    {
        std::string password = getPass.IOgetlineThrough(true);
        validate = db->isCorrectPassword(user->getId(), password);
        if (!validate)
        {
            std::cout << "Неверный пароль: " << std::endl;
            endInput = loginCancel.IOgetline();
            if (endInput == chat::yes)
            {
                return chat::login_cancel;
            }
        }
    } while (!validate);

    if (user->isBanned())
    {
        return chat::user_banned;
    }
    return chat::login_success;
}

void IChatInterface::pagination()
{
    if (pg_StartItem < 0)
    {
        pg_StartItem = 0;
    }
    if (pg_pageNumber < 1)
    {
        pg_pageNumber = 1;
    }
    // Если пустой массив
    if (pg_MaxItems == 0)
    {
        pg_StartItem = 0;
        pg_EndItem = 0;
        return;
    }

    // количество сообщений на страницу не должно превышать максимального количества сообщений
    if (pg_itemsPerPage >= pg_MaxItems)
    {
        pg_StartItem = 0;
        pg_EndItem = pg_MaxItems;
        return;
    }

    if (pg_itemsPerPage < 1)
    {
        pg_itemsPerPage = 1;
    }

    // максимально возможный номер страницы, урезается если введен превышающий диапазон
    pg_maxPageNumber = (pg_MaxItems / pg_itemsPerPage) + 1;
    if (pg_pageNumber > pg_maxPageNumber)
    {
        pg_pageNumber = pg_maxPageNumber;
    }

    // если запрошена страница
    if (paginationMode == page::page)
    {
        pg_StartItem = pg_itemsPerPage * (pg_pageNumber - 1);
        pg_EndItem = pg_StartItem + pg_itemsPerPage;
    }

    // если запрошен показ страницы от конкретного сообщения
    if (paginationMode == page::message)
    {
        pg_EndItem = pg_StartItem + pg_itemsPerPage;
    }

    // pg_StartItem + pg_itemsPerPage превысил максимум или запрошена последняя страница
    // будут отображаться последние pg_itemsPerPage сообщений
    if ((pg_EndItem > pg_MaxItems) || (paginationMode == page::last_page))
    {
        pg_StartItem = pg_MaxItems - pg_itemsPerPage;
        pg_EndItem = pg_MaxItems;
    }
}

void IChatInterface::pgDefault()
{
    paginationMode = page::last_page;
    pg_pageNumber = 1;
    pg_MaxItems = 0;
    pg_itemsPerPage = 10;
    pg_maxPageNumber = 0;
    pg_StartItem = 0;
    pg_EndItem = 0;
}

void IChatInterface::pgNavigation()
{
    UserInput<std::string, page::PaginationMode> selectOption(std::string(),
                                                              "Выберите опцию:"
                                                              "\nэнс - элементов на странице;"
                                                              "\nпнс - перейти на страницу...;"
                                                              "\nпкэ - перейти к элементу №...;"
                                                              "\nсбр - сброс настроек (всегда последние 10 элементов);"
                                                              "\nн - вернуться в чат;"
                                                              "\nВведите значение: ",
                                                              "Неверный ввод.", 5);
    selectOption.addInputs("энс", "пнс", "пкэ", "сбр", "н");
    selectOption.addOutputs(page::msg_per_page, page::page, page::message, page::last_page, page::close_options);

    UserInput<int, int> getInt(std::string(), std::string(), "Неверный ввод");

    switch (selectOption.IOgetline())
    {
    case page::msg_per_page:
        getInt.setMainMessage("Укажите количество элементов на странице (1 - " + std::to_string(pg_MaxItems) + "): ");
        pg_itemsPerPage = getInt.IOcinThrough();
        break;
    case page::page:
        getInt.setMainMessage("Укажите номер страницы (1 - " + std::to_string(pg_maxPageNumber) + "): ");
        pg_pageNumber = getInt.IOcinThrough();
        paginationMode = page::page;
        break;
    case page::message:
        getInt.setMainMessage("Укажите номер элемента (1 - " + std::to_string(pg_MaxItems) + "): ");
        pg_StartItem = getInt.IOcinThrough() - 1;
        paginationMode = page::message;
        break;
    case page::last_page:
        pgDefault();
    default:
        break;
    }
}

std::string IChatInterface::pgInfo()
{
    return std::to_string((pg_MaxItems <= 0) ? pg_StartItem : pg_StartItem + 1) + " - " +
           std::to_string(pg_EndItem) + " из " +
           std::to_string(pg_MaxItems);
}

std::string IChatInterface::StampToTime(long long timestamp)
{
    time_t tick = (time_t)(timestamp);
    struct tm tm;
    char s[40];
    tm = *localtime(&tick);
    strftime(s, sizeof(s), "%d.%m.%Y %H:%M:%S", &tm);
    std::string str(s);
    return str;
}

void IChatInterface::messagesList(std::unique_ptr<Message[]> messages)
{
    pagination();
    for (int i{pg_StartItem}; i < pg_EndItem && messages != nullptr; i++)
    {
        auto msgUser = db->getUserById(messages[i].getAuthorID());

        std::cout << std::endl;
        std::cout
            << i + 1 << ". "
            << StampToTime(messages[i].getDate()) + " "
            << msgUser->getUserName()
            << "[" << msgUser->getUserLogin() << "]"
            << "[userID " << std::to_string(msgUser->getId()) << "]"
            << (msgUser->isBanned() ? "[banned]" : std::string())
            << (msgUser->isAdmin() ? "[admin]" : std::string())
            << " [messageID " << messages[i].getId() << "] "
            << std::endl;
        std::cout << messages[i].getMessage() << std::endl;
    }
    std::cout << std::endl;
}

void IChatInterface::usersList(std::unique_ptr<User[]> users)
{
    pg_MaxItems = db->usersCount();
    pagination();
    for (int i{pg_StartItem}; i < pg_EndItem && users != nullptr; i++)
    {
        std::cout
            << i + 1 << ". "
            << users[i].getUserName()
            << "[" << users[i].getUserLogin() << "]"
            << "[userID " << std::to_string(users[i].getId()) << "]"
            << (users[i].isBanned() ? "[banned]" : std::string())
            << (users[i].isAdmin() ? "[admin]" : std::string())
            << std::endl;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void IChatInterface::userProfile()
{
    UserInput<std::string, user::options> options("Настройки пользователя. " + user->getUserName() + "[" + user->getUserLogin() + "]",
                                                  "Какие данные вы хотите поменять? (л - логин; п - пароль; и - имя; з - закончить): ",
                                                  "Неверный ввод",
                                                  4);
    options.addInputs("л", "п", "и", "з");
    options.addOutputs(user::login, user::pass, user::name, user::end);

    UserInput<std::string, std::string> getLogin("Изменение логина", "Введите логин: ", std::string());
    UserInput<std::string, std::string> getName("Изменение имени", "Введите имя: ", std::string());
    UserInput<std::string, std::string> getPass("Изменение пароля", "Введите пароль: ", std::string());
    UserInput<std::string, chat::Results> loginCancel(std::string(), "Отменить смену логина? (да - отменить, нет - продолжить): ", "Неверный ввод. Требуется: да или нет", 4);
    loginCancel.addInputs("да", "нет", "yes", "no");
    loginCancel.addOutputs(chat::yes, chat::no, chat::yes, chat::no);

    std::string loginTemp;
    std::string name;
    std::string pass;

    do
    {
        options.setDescription("Настройки пользователя. " + user->getUserName() + "[" + user->getUserLogin() + "]");
        user::options result = options.IOgetline();
        switch (result)
        {
        case user::login:
        {
            // ввод логина
            bool validLogin = false;
            do
            {
                loginTemp = getLogin.IOgetlineThrough(true);

                validLogin = db->isUniqueLogin(loginTemp);
                if (!validLogin)
                {
                    std::cout << "Этот логин занят!" << std::endl;
                    if (loginCancel.IOgetline() == chat::yes)
                    {
                        break;
                    }
                }
                user->setUserLogin(loginTemp);
                db->updateUserData(*user);

            } while (!validLogin);
        }
        break;
        case user::name:
            name = getName.IOgetlineThrough(true);
            user->setUserName(name);
            db->updateUserData(*user);
            break;

        case user::pass:
            pass = getPass.IOgetlineThrough(true);
            db->setUserPassword(user->getId(), pass);
            break;
        case user::end:
            return;
        default:
            break;
        }

    } while (1);
}