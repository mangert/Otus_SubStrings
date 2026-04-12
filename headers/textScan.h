#pragma once

#include <string>
#include <optional>

/**
 * @namespace textScan
 * @brief Алгоритмы поиска подстроки
 */
namespace textScan {

    /**
     * @brief Полный перебор. Сложность O(n*m)
     * @param text Текст
     * @param pattern Шаблон для поиска
     * @return Позиция первого вхождения или nullopt
     */
    std::optional<size_t> naiveSearch(std::string_view text, std::string_view pattern);

    /**
     * @brief Алгоритм Кнута-Морриса-Пратта (сдвиги по префиксу). Сложность O(n+m)
     * @param text Текст
     * @param pattern Шаблон для поиска
     * @return Позиция первого вхождения или nullopt
     */
    std::optional<size_t> KMPSearch(std::string_view text, std::string_view pattern);

    /**
     * @brief Алгоритм Бойера-Мура-Хорспула (сдвиги по суффиксу текста)
     * @note Упрощённая версия Бойера-Мура, только таблица плохих символов
     * @param text Текст
     * @param pattern Шаблон для поиска
     * @return Позиция первого вхождения или nullopt
     */
    std::optional<size_t> BMHSearch(std::string_view text, std::string_view pattern);

    /**
     * @brief Полный алгоритм Бойера-Мура (плохой символ + хороший суффикс)
     * @note Самый эффективный для больших текстов и длинных шаблонов
     * @param text Текст
     * @param pattern Шаблон для поиска
     * @return Позиция первого вхождения или nullopt
     */
    std::optional<size_t> BMSearch(std::string_view text, std::string_view pattern);

} // namespace textScan