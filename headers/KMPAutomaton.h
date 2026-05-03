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
    * @brief Определяет префикс-функцию для автомата
    */
    enum class PrefixMode { SLOW, FAST };
    
    template<PrefixMode mode>
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
         * @brief Возвращает текущее состояние (сколько символов совпало)
         */
        size_t currentState() const { return currentState_; }
        
        /**
         * @brief Возвращает текущее состояние (сколько символов совпало)
         */
        const std::string& getPattern() { return pattern_; }

        /**
         * @brief Визуализация таблицы переходов
         */
        void printTransitionTable() const;

    private:
        std::string pattern_;  // Шаблон        
        size_t patternLen_;    // длина шаблона
        size_t currentState_;  // Текущее состояние
        std::vector<std::unordered_map<char, size_t>> transitions_; //таблица переходов
        
        std::vector<size_t> buildPrefixFunction() const; //Построение префикс-функции
        void buildTransitionTable(); // Построение таблицы        
        size_t getTransition(size_t state, char c) const; // Получение перехода из таблицы
    };

    // +++++++++++++++ Реализация методов класса ++++++++++++++++++ //
    
    // ============== Конструктор ===============    
    template<PrefixMode mode>
    KMPAutomaton<mode>::KMPAutomaton(std::string_view pattern)
        : pattern_(pattern)
        , patternLen_(pattern.length())
        , currentState_(0)
    {
        if (patternLen_ > 0) {
            buildTransitionTable();
        }
    }
    // ============== Построение префикс-функции ===============
    template<> // быстрая реализация
    inline std::vector<size_t> KMPAutomaton<PrefixMode::FAST>::buildPrefixFunction() const {

        std::vector<size_t> pi(patternLen_, 0);
        for (size_t i = 1; i != patternLen_; ++i) {
            size_t j = pi[i - 1];
            while (j > 0 && pattern_[i] != pattern_[j]) {
                j = pi[j - 1];
            }
            if (pattern_[i] == pattern_[j]) {
                ++j;
            }
            pi[i] = j;
        }
        return pi;
    }
    
    template<> // медленная реализация
    inline std::vector<size_t> KMPAutomaton<PrefixMode::SLOW>::buildPrefixFunction() const {
        size_t m = pattern_.length();
        std::vector<size_t> pi(m, 0);

        for (size_t i = 1; i < m; ++i) {
            // Пытаемся все возможные длины от i до 1
            for (size_t k = i; k > 0; --k) {
                // Проверяем, совпадает ли префикс длины k с суффиксом длины k
                bool match = true;
                for (size_t j = 0; j < k; ++j) {
                    if (pattern_[j] != pattern_[i - k + 1 + j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    pi[i] = k;
                    break;  // нашли наибольший
                }
            }
        }

        return pi;
    }
    // ============== Построение сжатой таблицы переходов ===============
    template<PrefixMode mode>
    void KMPAutomaton<mode>::buildTransitionTable() {

        std::vector<size_t> pi = buildPrefixFunction();

        // 2. Инициализируем сжатую таблицу
        transitions_.resize(patternLen_ + 1);

        // Для каждого состояния
        for (size_t state = 0; state <= patternLen_; ++state) {

            // Если мы не в терминальном состоянии и следующий символ совпадает
            if (state < patternLen_) {
                char matchChar = pattern_[state];
                transitions_[state][matchChar] = state + 1;  // переход вперёд
            }

            //Берём переходы из fallback-состояния            
            if (state != 0) {
                size_t fallbackState = pi[state - 1];  // куда откатиться при несовпадении

                // Копируем переходы из fallback-состояния
                for (const auto& [ch, nextState] : transitions_[fallbackState]) {
                    // не затираем переход, который уже есть
                    if (transitions_[state].find(ch) == transitions_[state].end()) {
                        transitions_[state][ch] = nextState;
                    }
                }
            }
        }
    }


    // ============== Получение перехода (с default) ===============
    template<PrefixMode mode>
    size_t KMPAutomaton<mode>::getTransition(size_t state, char c) const {
        auto it = transitions_[state].find(c);
        if (it != transitions_[state].end()) {
            return it->second;
        }
        //если нет явного перехода - состояние 0
        return 0;
    }


    // ============== Поиск в тексте ===============
    template<PrefixMode mode>
    std::optional<size_t> KMPAutomaton<mode>::search(std::string_view text) const {

        if (patternLen_ == 0) return 0;
        if (patternLen_ > text.length()) return std::nullopt;

        size_t state = 0;

        for (size_t i = 0; i < text.length(); ++i) {
            state = getTransition(state, text[i]);

            if (state == patternLen_) {
                return i - patternLen_ + 1;
            }
        }

        return std::nullopt;
    }


    // ============== Пошаговое прохождение ===============
    template<PrefixMode mode>
    size_t KMPAutomaton<mode>::next(char c) {
        currentState_ = getTransition(currentState_, c);
        return currentState_;
    }
    
    template<PrefixMode mode>
    void KMPAutomaton<mode>::reset() {
        currentState_ = 0;
    }


    // ============== Визуализация таблицы ===============
    template<PrefixMode mode>
    void KMPAutomaton<mode>::printTransitionTable() const {
        std::cout << "\n=== Таблица переходов КМП-автомата ===\n";
        std::cout << "Шаблон: \"" << pattern_ << "\" (длина " << patternLen_ << ")\n\n";

        for (size_t state = 0; state <= patternLen_; ++state) {
            std::cout << "Состояние " << state << ":\n";

            if (transitions_[state].empty()) {
                std::cout << "  (все переходы ведут в 0)\n";
            }
            else {
                for (const auto& [ch, nextState] : transitions_[state]) {
                    // Экранируем непечатные символы
                    if (ch >= 32 && ch <= 126) {
                        std::cout << "  '" << ch << "' -> " << nextState << "\n";
                    }
                    else {
                        std::cout << "  \\x" << std::hex << static_cast<int>(ch)
                            << std::dec << " -> " << nextState << "\n";
                    }
                }
            }
            std::cout << "\n";
        }
    }
} // namespace textScan
