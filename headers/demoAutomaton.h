#pragma once
#include <iostream>
#include <string_view>
#include <vector>
#include <array>
#include <optional>
#include <chrono>
#include "KMPAutomaton.h"

template<textScan::PrefixMode mode>
void demonstrateAutomatonWalk(textScan::KMPAutomaton<mode>& automaton, std::string_view text) {
    std::cout << "\n=== Пошаговое прохождение автомата ===\n";
    std::cout << "Шаблон: \"" << automaton.getPattern() << "\"\n";
    std::cout << "Текст:  \"" << text << "\"\n";
    std::cout << "----------------------------------------\n";

    automaton.reset();

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        size_t newState = automaton.next(c);

        std::cout << "Шаг " << i << ": символ '" << c << "' - состояние " << newState;
        
        if (newState == automaton.getPattern().length()) {
            std::cout << " НАШЛИ! (позиция " << (i - newState + 1) << ")";
        }
        std::cout << "\n";
        
    }
    std::cout << "========================================\n\n";
}

