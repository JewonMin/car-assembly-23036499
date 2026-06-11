#include "../../include/step1_type_selection/CarTypeSelector.h"
#include "../../include/shared/utils.h"
#include <cstdio>

CarTypeSelector::CarTypeSelector(CarTypes carTypes)
    : carTypes_(std::move(carTypes)) {}

bool CarTypeSelector::select(Car& car) {
    printf(CLEAR_SCREEN);
    printf("        ______________\n");
    printf("       /|            | \n");
    printf("  ____/_|_____________|____\n");
    printf(" |                      O  |\n");
    printf(" '-(@)----------------(@)--'\n");
    printf("===============================\n");
    printf("어떤 차량 타입을 선택할까요?\n");

    int count = 0;
    for (auto& p : carTypes_)
        printf("%d. %s\n", ++count, p->name().c_str());
    printf("===============================\n");

    int answer = readInput(1, count);
    if (answer == EXIT_SIGNAL) return false;

    car.type = carTypes_[answer - 1];
    printf("%s 차량 타입을 선택하셨습니다.\n", car.type->name().c_str());
    delayMs(800);
    return true;
}
