#include <iostream>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <functional>
#include <iomanip>
#include <random>
#include <string_view>
#include "textScan.h"

/**
 * @brief Красиво выводит строку, сокращая если она слишком длинная
 * @param str Исходная строка
 * @param maxLen Максимальная длина вывода (по умолчанию 50)
 * @return Строка с возможным сокращением
 */
std::string truncateString(std::string_view str, size_t maxLen = 50) {
    if (str.length() <= maxLen) {
        return std::string(str);
    }

    size_t prefixLen = maxLen / 2;      // показываем начало
    size_t suffixLen = maxLen - prefixLen - 3; // и конец

    std::string result;
    result.reserve(maxLen + 3);
    result.append(str.data(), prefixLen);
    result.append("...");
    result.append(str.data() + str.length() - suffixLen, suffixLen);

    return result;
}

/**
 * @brief Выводит информацию о тесте (текст и шаблон в сокращённом виде)
 */
void printTestInfo(std::string_view text, std::string_view pattern, size_t T) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ТЕСТОВЫЙ НАБОР:\n";

    // Выводим текст с умным сокращением
    std::cout << "Текст:   \"" << truncateString(text, 60) << "\"";
    if (text.length() > 60) {
        std::cout << " (всего " << text.length() << " байт)";
    }
    std::cout << "\n";

    // Выводим шаблон с умным сокращением
    std::cout << "Шаблон:  \"" << truncateString(pattern, 40) << "\"";
    if (pattern.length() > 40) {
        std::cout << " (всего " << pattern.length() << " байт)";
    }
    std::cout << "\n";

    std::cout << "Итераций: " << T << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

/**
 * @brief Одиночный тест: замеряет время одного поиска
 * @return Пара (время выполнения, позиция вхождения)
 */
std::pair<std::chrono::microseconds, std::optional<size_t>>
single_test(std::string_view text, std::string_view pattern,
    std::function<std::optional<size_t>(std::string_view, std::string_view)> search) {

    auto start = std::chrono::high_resolution_clock::now();
    std::optional<size_t> pos = search(text, pattern);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return { duration, pos };
}

/**
 * @brief Многократный тест: усредняет время T повторений
 */
void searchTest(std::string_view text, std::string_view pattern,
    std::function<std::optional<size_t>(std::string_view, std::string_view)> search,
    const std::string& name, size_t T) {

    std::cout << "========================================\n";
    std::cout << "Тестирование алгоритма: " << name << "\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Длина текста:   " << text.length() << " байт\n";
    std::cout << "Длина шаблона:  " << pattern.length() << " байт\n";
    std::cout << "Число итераций: " << T << "\n";

    // Показываем фрагменты текста и шаблона (если они не слишком короткие)
    if (text.length() > 20) {
        std::cout << "Начало текста: \"" << truncateString(text, 40) << "\"\n";
    }
    if (pattern.length() > 20) {
        std::cout << "Начало шаблона: \"" << truncateString(pattern, 40) << "\"\n";
    }

    std::cout << "----------------------------------------\n";

    std::chrono::microseconds total_time(0);
    std::optional<size_t> last_result;

    for (size_t i = 0; i < T; ++i) {
        auto [duration, pos] = single_test(text, pattern, search);
        total_time += duration;
        last_result = pos;
    }

    auto avg_time = total_time / static_cast<long long>(T);

    std::string result_str;
    if (last_result.has_value()) {
        result_str = "Найдено на позиции " + std::to_string(*last_result);
    }
    else {
        result_str = "Не найдено";
    }

    std::cout << "Результат:      " << result_str << "\n";
    std::cout << "Среднее время:  " << avg_time.count() << " мкс\n";
    std::cout << "========================================\n\n";
}

/**
 * @brief Обёртка для тестирования всех алгоритмов на одном наборе данных
 */
void testAllAlgorithms(std::string_view text, std::string_view pattern, size_t T,
    bool testNaive = true,
    bool testKMP = true,
    bool testBMH = true,
    bool testBM = true) {

    // Выводим информацию о тесте (сокращённо)
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ТЕСТОВЫЙ НАБОР:\n";
    std::cout << "Текст:   \"" << truncateString(text, 60) << "\"";
    if (text.length() > 60) {
        std::cout << " (всего " << text.length() << " байт)";
    }
    std::cout << "\n";
    std::cout << "Шаблон:  \"" << truncateString(pattern, 40) << "\"";
    if (pattern.length() > 40) {
        std::cout << " (всего " << pattern.length() << " байт)";
    }
    std::cout << "\n";
    std::cout << "Итераций: " << T << "\n";
    std::cout << std::string(60, '=') << "\n\n";

    if (testNaive) {
        searchTest(text, pattern, textScan::naiveSearch, "Полный перебор", T);
    }
    if (testKMP) {
        searchTest(text, pattern, textScan::KMPSearch, "КМП (префикс)", T);
    }
    if (testBMH) {
        searchTest(text, pattern, textScan::BMHSearch, "Бойер-Мур-Хорспул", T);
    }
    if (testBM) {
        searchTest(text, pattern, textScan::BMSearch, "Бойер-Мур (полный)", T);
    }
}



/**
 * @brief Генерирует случайный текст заданной длины
 */
std::string generateRandomText(size_t length, const std::string& alphabet = "ACGT") {
    std::string result;
    result.reserve(length);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, alphabet.size() - 1);

    for (size_t i = 0; i < length; ++i) {
        result += alphabet[dis(gen)];
    }

    return result;
}
