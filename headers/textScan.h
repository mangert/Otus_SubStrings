#pragma once

#include <string>
#include <vector>
#include <unordered_map>
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

    // Дополлнение - in construction
    class KMPAutomaton {
    public:
        /**
         * @brief Конструирует автомат для заданного шаблона
         * @param pattern Шаблон для поиска
         */
        explicit KMPAutomaton(std::string_view pattern);

        KMPAutomaton() = default;
        KMPAutomaton(const KMPAutomaton& other) = default;
        KMPAutomaton& operator=(const KMPAutomaton& other) = default;
        KMPAutomaton(KMPAutomaton&& other) = default;
        KMPAutomaton& operator=(KMPAutomaton&& other) = default;

        ~KMPAutomaton() = default;

        /**
         * @brief Поиск первого вхождения в тексте
         * @param text Текст для поиска
         * @return Позиция первого вхождения или std::nullopt
         */
        std::optional<size_t> search(std::string_view text) const;

        /**
         * @brief Пошаговый поиск (для демонстрации работы автомата)
         * @param c Очередной символ текста
         * @return Текущее состояние (длина совпавшего префикса)
         */
        size_t next(char c);

        /**
         * @brief Сброс автомата в начальное состояние
         */
        void reset();

        /**
         * @brief Визуализация таблицы переходов (для отладки)
         */
        void printTransitionTable() const;        

    private:
        std::string pattern_;                          // Шаблон
        size_t patternLen_;                            // Длина шаблона
        size_t currentState_;                          // Текущее состояние
        std::vector<std::unordered_map<char, size_t>> transitions_; //таблица переходов
                                            

        void buildTransitionTable();                             // Построение таблицы
        size_t computeTransition(size_t state, char c) const;    // Вычисляет переход (без таблицы)
        size_t getTransition(size_t state, char c) const;        // Получает переход из таблицы
    };

} // namespace textScan