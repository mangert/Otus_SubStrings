#include <iostream>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <functional>
#include <iomanip>
#include "textScan.h"

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
    std::cout << "Длина текста:   " << text.length() << " байт\n";
    std::cout << "Длина шаблона:  " << pattern.length() << " байт\n";
    std::cout << "Число итераций: " << T << "\n";
    std::cout << "----------------------------------------\n";

    std::chrono::microseconds total_time(0);
    std::optional<size_t> last_result;

    for (size_t i = 0; i < T; ++i) {
        auto [duration, pos] = single_test(text, pattern, search);
        total_time += duration;
        last_result = pos;  // предполагаем, что результат одинаковый
    }

    auto avg_time = total_time / static_cast<long long>(T);

    // Формируем вывод результата
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

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ТЕСТОВЫЙ НАБОР:\n";
    std::cout << "Текст:   \"" << text << "\"\n";
    std::cout << "Шаблон:  \"" << pattern << "\"\n";
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