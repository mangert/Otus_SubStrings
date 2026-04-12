#include <iostream>
#include <string>
#include "textScan.h"

void testAllAlgorithms(std::string_view text, std::string_view pattern, size_t T,
    bool testNaive = true,
    bool testKMP = true,
    bool testBMH = true,
    bool testBM = true);

int main() {
    setlocale(LC_ALL, "russian");
    // Малые тесты (много итераций)
    const size_t SMALL_ITER = 10000;
    // Средние тесты
    const size_t MEDIUM_ITER = 1000;
    // Большие тесты (мало итераций)
    const size_t LARGE_ITER = 10;

    // Тест 1: Короткий текст, паттерн в начале
    testAllAlgorithms("Hello World!", "Hello", SMALL_ITER);

    // Тест 2: Паттерн в середине
    testAllAlgorithms("Hello World!", "World", SMALL_ITER);

    // Тест 3: Паттерн не найден
    testAllAlgorithms("Hello World!", "xyz", SMALL_ITER);

    // Тест 4: Пустой паттерн
    testAllAlgorithms("Hello World!", "", SMALL_ITER);

    // Тест 5: Паттерн длиннее текста
    testAllAlgorithms("Hi", "Hello", SMALL_ITER);

    // Тест 6: Повторяющиеся символы (худший случай для naive)
    testAllAlgorithms("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaab", MEDIUM_ITER);

    // Тест 7: Большой текст (например, сгенерируй или возьми реальный)
    std::string longText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.";
    testAllAlgorithms(longText, "consectetur", LARGE_ITER);

    // Тест 8: Русский текст (проверка UTF-8)
    testAllAlgorithms("Привет мир!", "мир", SMALL_ITER);

    return 0;
}