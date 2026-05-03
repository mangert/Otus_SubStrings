#include <iostream>
#include <string>
#include "textScan.h"
#include "demoAutomaton.h"


void testAllAlgorithms(std::string_view text, std::string_view pattern, size_t T,
    bool testNaive = true,
    bool testKMP = true,
    bool testBMH = true,
    bool testBM = true,
    bool testSlowAutomaton = true,
    bool testFastAutomaton = true);

std::string generateRandomText(size_t length, const std::string& alphabet = "ACGT");

int main() {
    setlocale(LC_ALL, "russian");
    // Малые тесты (много итераций)
    const size_t SMALL_ITER = 10000;
    // Средние тесты
    const size_t MEDIUM_ITER = 1000;
    // Большие тесты (мало итераций)
    const size_t LARGE_ITER = 10;

    // Тест 1: Короткий текст, паттерн в начале
    testAllAlgorithms("Hello World!", "Hello", SMALL_ITER, false, true, false, false, true, true);

    // Тест 2: Паттерн в середине
    testAllAlgorithms("Hello World!", "World", SMALL_ITER, false, true, false, false, true, true);

    // Тест 3: Паттерн не найден
    testAllAlgorithms("Hello World!", "xyz", SMALL_ITER, false, true, false, false, true, true);

    // Тест 4: Пустой паттерн
    testAllAlgorithms("Hello World!", "", SMALL_ITER, false, true, false, false, true, true);

    // Тест 5: Паттерн длиннее текста
    testAllAlgorithms("Hi", "Hello", SMALL_ITER, false, true, false, false, true, true);

    // Тест 6: Повторяющиеся символы (худший случай для naive)
    testAllAlgorithms("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaab", MEDIUM_ITER, false, true, false, false, true, true);

    // Тест 7: Большой текст
    std::string alphabet = " QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?!@#$%^&*()_+1234567890-=qwertyuiop[]asdfghjkl;'\\zxcvbnm,./\n\t";
    size_t len = 500000;
    std::string longText = generateRandomText(len, alphabet);
    size_t pattern_len = 100;
    std::string pattern = longText.substr(len - pattern_len + 1, pattern_len);
    testAllAlgorithms(longText, pattern, LARGE_ITER, false, true, false, false, true, true);

    // Тест 8: Русский текст (проверка UTF-8)
    testAllAlgorithms("Привет мир!", "мир", SMALL_ITER, false, true, false, false, true, true);

    // Демонстрация прохождения конечного автомата
    std::string a_text = "ABABCFKLATGKABABCKDKJG";
    std::string a_pattern = "ABABC";
    std::cout << "\n=========================================================\n";
    std::cout << "========= Пошаговое прохождение конечного автомата ========\n\n";
    std::cout << "Tекст: " << a_text << "\n";
    std::cout << "Шаблон: " << a_pattern << "\n";
    textScan::KMPAutomaton<textScan::PrefixMode::SLOW> automat(a_pattern);
    demonstrateAutomatonWalk(automat, a_text);
    
   return 0;
}