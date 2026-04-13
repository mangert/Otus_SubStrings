#include <iostream>
#include <string_view>
#include <vector>
#include <array>
#include <optional>

namespace textScan {

    namespace {
        // Построение префикс-функции для шаблона (для КМП)
        std::vector<size_t> buildPrefixFunction(std::string_view pattern) {
            size_t m = pattern.length();
            std::vector<size_t> pi(m, 0); 

            for (size_t i = 1; i != m; ++i) {
                size_t j = pi[i - 1];

                // Пока не совпадает и не дошли до начала
                while (j > 0 && pattern[i] != pattern[j]) {
                    j = pi[j - 1];
                }

                // Если совпало, увеличиваем длину префикса
                if (pattern[i] == pattern[j]) {
                    ++j;
                }

                pi[i] = j;
            }

            return pi;
        }

    } //anonymos namespace
    
    // Полный перебор
    std::optional<size_t> naiveSearch(std::string_view text, std::string_view pattern) {
        if (pattern.empty()) return 0;  // Пустой паттерн всегда найден в позиции 0

        const size_t textLen = text.length();
        const size_t patternLen = pattern.length();

        if (patternLen > textLen) return std::nullopt;

        size_t maxStart = textLen - patternLen;

        for (size_t start_pos = 0; start_pos <= maxStart; ++start_pos) {
            size_t offset = 0;
            while (offset < patternLen
                && text[start_pos + offset] == pattern[offset]) {                
                ++offset;
            }
            if (offset == patternLen) {
                return start_pos;
            }
        }
        return std::nullopt;
    }

    // Поиск подстроки по префиксу шаблона (Кнут-Моррис-Пратт) 
    std::optional<size_t> KMPSearch(std::string_view text, std::string_view pattern) {
        // Проверка граничных случаев
        if (pattern.empty()) return 0;

        const size_t textLen = text.length();
        const size_t patternLen = pattern.length();

        if (patternLen > textLen) return std::nullopt;

        //строим префикс-функцию для шаблона
        std::vector<size_t> pi = buildPrefixFunction(pattern);

        //поиск
        size_t j = 0;  // текущая длина совпавшего префикса

        for (size_t i = 0; i != textLen; ++i) {
            // Пока не совпадает и не в начале
            while (j > 0 && text[i] != pattern[j]) {
                j = pi[j - 1];  // Сдвиг по префиксу
            }

            // Если символы совпали
            if (text[i] == pattern[j]) {
                ++j;
            }

            // Если нашли полное совпадение
            if (j == patternLen) {
                return i - patternLen + 1;
            }
        }

        return std::nullopt;
    }

    // Поиск подстроки по суффиксу шаблона (Бойер-Мур-Хорспул) 
    std::optional<size_t> BMHSearch(std::string_view text, std::string_view pattern) {
        if (pattern.empty()) return 0;                       
        
        const size_t textLen = text.length();
        const size_t patternLen = pattern.length();
        
        if (patternLen > textLen) return std::nullopt;

        // Таблица сдвигов
        std::array<size_t, 256> shift;
        for (size_t i = 0; i < 256; ++i) 
            shift[i] = patternLen;
        for (size_t i = 0; i < patternLen - 1; ++i) {
            shift[static_cast<unsigned char>(pattern[i])] = patternLen - 1 - i;
        }

        // Поиск
        size_t i = patternLen - 1;  // позиция в тексте, на которую смотрим

        while (i < textLen) {
            size_t j = 0;
            // Сравниваем справа налево
            while (j < patternLen && text[i - j] == pattern[patternLen - 1 - j]) {
                ++j;
            }

            if (j == patternLen) {
                return i - patternLen + 1;  // нашли
            }

            // Сдвигаемся по таблице
            unsigned char c = static_cast<unsigned char>(text[i]);
            i += shift[c];
        }

        return std::nullopt;
    }

    // Поиск подстроки по Бойер-Муру
    std::optional<size_t> BMSearch(std::string_view text, std::string_view pattern) {
        
        if (pattern.empty()) return 0;
        
        const size_t textLen = text.length();
        const size_t patternLen = pattern.length();
        
        if (patternLen > textLen) return std::nullopt;
        
        // ===== 1. ТАБЛИЦА ПЛОХИХ СИМВОЛОВ =====
        std::array<size_t, 256> badCharShift;
        for (size_t i = 0; i < 256; ++i) {
            badCharShift[i] = patternLen;
        }
        for (size_t i = 0; i < patternLen - 1; ++i) {
            badCharShift[static_cast<unsigned char>(pattern[i])] = patternLen - 1 - i;
        }

        // ===== 2. ТАБЛИЦА ХОРОШИХ СУФФИКСОВ =====
        std::vector<size_t> goodSuffixShift(patternLen + 1, 0);
        std::vector<size_t> border(patternLen + 1, 0);

        // Построение границ (borders) для суффиксов
        size_t i = patternLen;
        size_t j = patternLen + 1;
        border[i] = j;

        while (i > 0) {
            while (j <= patternLen && pattern[i - 1] != pattern[j - 1]) {
                if (goodSuffixShift[j] == 0) {
                    goodSuffixShift[j] = j - i;
                }
                j = border[j];
            }
            --i;
            --j;
            border[i] = j;
        }

        // Заполнение оставшихся сдвигов
        j = border[0];
        for (i = 0; i <= patternLen; ++i) {
            if (goodSuffixShift[i] == 0) {
                goodSuffixShift[i] = j;
            }
            if (i == j) {
                j = border[j];
            }
        }

        // ===== 3. ПОИСК =====
        size_t pos = 0;  // позиция в тексте

        while (pos <= textLen - patternLen) {
            // Сравниваем справа налево, j = количество оставшихся символов
            size_t j = patternLen;

            while (j != 0 && text[pos + j - 1] == pattern[j - 1]) {
                --j;
            }

            if (j == 0) {
                return pos;  // все символы совпали
            }

            // j > 0, несовпадение на позиции j-1
            unsigned char badChar = static_cast<unsigned char>(text[pos + j - 1]);
            size_t badShift = badCharShift[badChar];
            size_t goodShift = goodSuffixShift[j];  // таблица для позиции j

            pos += std::max(badShift, goodShift);
        }

        return std::nullopt;
    }
    

}//namespace textScan