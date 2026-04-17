#include "headers/textScan.h"
#include <algorithm>

namespace textScan {

	// ============== Конструкторы ===============
	KMPAutomaton::KMPAutomaton(std::string_view pattern) pattern_(pattern)
		, patternLen_(pattern.length()), currentState_(0) {
		
		buildTransitionTable();
	};	


	// ============== Служебные функции ===============

    size_t KMPAutomaton::getTransition(size_t state, char c) const {
        auto it = transitions_[state].find(c);
        if (it != transitions_[state].end()) {
            return it->second;
        }
        // Default: если нет явного перехода -> состояние 0
        return 0;
    }

    void KMPAutomaton::buildTransitionTable() {
        if (patternLen_ == 0) return;

        // 1. Сначала строим префикс-функцию (быстрый вариант)
        std::vector<size_t> pi(patternLen_, 0);
        for (size_t i = 1; i < patternLen_; ++i) {
            size_t j = pi[i - 1];
            while (j > 0 && pattern_[i] != pattern_[j]) {
                j = pi[j - 1];
            }
            if (pattern_[i] == pattern_[j]) {
                ++j;
            }
            pi[i] = j;
        }

        // 2. Инициализируем сжатую таблицу
        transitions_.resize(patternLen_ + 1);

        // 3. Для каждого состояния заполняем только НЕ-deafult переходы
        for (size_t state = 0; state <= patternLen_; ++state) {
            // Добавляем переходы только для символов, которые реально меняют состояние

            // Случай 1: если state < patternLen_, то переход по pattern[state] ведёт в state+1
            if (state < patternLen_) {
                char matchChar = pattern_[state];
                transitions_[state][matchChar] = state + 1;
            }

            // Случай 2: переходы по символам, которые не совпадают, но ведут не в 0
            // Это нужно только для не-нулевых состояний
            if (state > 0) {
                size_t fallbackState = pi[state - 1];

                // Берём все переходы из fallbackState и копируем те,
                // которые не ведут в то же состояние, что и default
                for (const auto& [ch, nextState] : transitions_[fallbackState]) {
                    // Не копируем переход, который и так есть (например, совпадающий символ)
                    if (transitions_[state].find(ch) == transitions_[state].end()) {
                        transitions_[state][ch] = nextState;
                    }
                }
            }
        }

        // Оптимизация: удаляем переходы, которые ведут в 0 (можно не хранить)
        for (size_t state = 0; state <= patternLen_; ++state) {
            auto it = transitions_[state].begin();
            while (it != transitions_[state].end()) {
                if (it->second == 0) {
                    it = transitions_[state].erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }

}