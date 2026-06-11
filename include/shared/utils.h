#pragma once
#include <cstdio>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>

constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";
constexpr int         EXIT_SIGNAL  = INT_MIN;

inline void delayMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline int readInput(int minVal, int maxVal) {
    std::string input;
    while (true) {
        printf("INPUT > ");
        std::getline(std::cin, input);
        if (input == "exit") return EXIT_SIGNAL;

        int  answer = 0;
        bool isNum  = false;
        try { answer = std::stoi(input); isNum = true; } catch (...) {}

        if (!isNum)
            printf("ERROR :: 숫자만 입력 가능\n");
        else if (answer < minVal || answer > maxVal)
            printf("ERROR :: %d ~ %d 범위만 선택 가능\n", minVal, maxVal);
        else
            return answer;

        delayMs(800);
    }
}
