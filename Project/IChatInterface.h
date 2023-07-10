
#pragma once
#include "DB.h"
#include <iostream>
#include <memory>
#include "UserInput.h"
#include "EnumOptions.h"

const extern char clear[];

class IChatInterface
{
protected:
    std::unique_ptr<User> user = nullptr;

    // настройки которые управляются из метода pagination()
    page::PaginationMode paginationMode = page::last_page;
    int pg_pageNumber = 1;
    int pg_MaxItems = 0;
    int pg_itemsPerPage = 10;
    int pg_maxPageNumber = 0;
    int pg_StartItem = 0;
    int pg_EndItem = 0;

public:
    std::unique_ptr<DB> db;
    IChatInterface() = default;
    virtual ~IChatInterface() = default;
    virtual chat::Results run(std::unique_ptr<DB> _db) = 0;

    /// @brief Проверяет логин и пароль при входе пользователя.
    /// @return
    chat::Results login();

    /*
        pagination это процесс разбития списка элементов на страницы по n элементов.
        Позволяет выводить списки постранично или запрашивать определенные диапазоны элементов.
    */

    /// @brief Вычисляет начальный и конечный индексы диапазона в массиве сообщений
    /// в зависимости от максимального количества элементов и элементов на странице.
    /// Результаты сохраняет в переменные класса: pg_maxPageNumber, pg_StartItem, pg_EndItem
    /// Исходные данные берет из переменных pg_MaxItems, pg_itemsPerPage, pg_pageNumber
    void pagination();

    /*
    Перед вызовом pagination() надо задать значения переменным класса: pg_MaxItems, pg_itemsPerPage, pg_pageNumber
    paginationMode = page - будет рассчитан диапазон для указанной страницы
    paginationMode = message - рассчитает диапазон pg_StartItem + pg_itemsPerPage
    paginationMode = last_page - рассчитает диапазон последних pg_itemsPerPage в списке
    Если суммирование pg_itemsPerPage выходит за пределы максимального количества элементов списка, диапазон будет урезан.
    После вызова pagination() будут вычислены значения переменных pg_maxPageNumber, pg_StartItem, pg_EndItem
    Вывести список после пагинации:
    for (int i = pg_StartItem; i < pg_EndItem; i++)
    {
        std::cout << list[i] << std::endl;
    }
    */

    /// @brief Сброс настроек пагинации (Вывод списка последних 10 элементов)
    void pgDefault();

    /// @brief Интерактивное меню настроек пагинации. Запрашивает у пользователя данные для отображения диапазона элементов.
    void pgNavigation();

    /// @brief Возвращает информацию о количестве элементов в списке
    std::string pgInfo();

    /// @brief Преобразует timestamp в дату/время
    /// @param timestamp
    /// @return
    std::string StampToTime(long long timestamp);

    /// @brief Выводит на экран список сообщений
    /// @param messages массив сообщений
    void messagesList(std::unique_ptr<Message[]> messages = nullptr);

    /// @brief Выводит на экран список пользователей
    /// @param users массив пользователей
    void usersList(std::unique_ptr<User[]> users = nullptr);

    /// @brief Реализует страницу редактирования профиля пользователя.
    void userProfile();
};