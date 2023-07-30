
#pragma once
#include <iostream>
#include "IChatInterface.h"
#include "ChatUserInterface.h"
#include "ChatAdminInterface.h"
#include "UserInput.h"
#include "ChatClient.h"
class Chat
{
private:
    Chat() = default;

public:
    ~Chat() = default;
    static void run();
};

void Chat::run()
{
    std::unique_ptr<DB> db = std::make_unique<DB>();
    db->startClient();

    ChatUserInterface userInterface;
    ChatAdminInterface adminInterface;

    chat::Results userInput;
    chat::Results result = chat::empty;
    // Объект страницы
    UserInput<std::string, chat::Results> coreAreaPage("Главная станица",
                                                       "Выберите действия: ч - Чат, а - Раздел администратора, в - Выход из программы ",
                                                       "Неверный ввод",
                                                       3 // количество возможных вариантов ввода
    );

    // создание возможных вариантов ввода
    coreAreaPage.addInputs("ч", "а", "в");
    // Соответствующие варианты вывода
    coreAreaPage.addOutputs(chat::chat, chat::admin, chat::app_exit);

    do
    {
        userInput = coreAreaPage.IOgetline();
        switch (userInput)
        {
        case chat::chat:
            system(clear);
            result = userInterface.run(std::move(db));
            db = std::move(userInterface.db);
            break;
        case chat::admin:
            result = adminInterface.run(std::move(db));
            db = std::move(adminInterface.db);
            break;
        case chat::app_exit:
            result = chat::app_exit;
            break;
        default:
            std::cout << "Неверный ввод" << std::endl;
            break;
        }
    } while (result != chat::app_exit);
    std::cout << "Вы вышли из чата. До свидания." << std::endl;
   // db->saveToFileData();
}
