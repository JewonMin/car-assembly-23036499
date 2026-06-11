#include "../../include/step2_part_assembly/CarPartAssembler.h"
#include "../../include/shared/utils.h"
#include <cstdio>

CarPartAssembler::CarPartAssembler(Engines e, Brakes b, Steerings s)
    : engines_(std::move(e)), brakeSystems_(std::move(b)), steeringSystems_(std::move(s)) {}

PartAssembleResult CarPartAssembler::assemble(Car& car) {
    int step = 0;  // 0=엔진, 1=제동장치, 2=조향장치
    while (step < 3) {
        int answer = showStep(step);
        if (answer == EXIT_SIGNAL)         return PartAssembleResult::Exit;
        if (answer == 0 && step == 0)      return PartAssembleResult::BackToTypeSelect;
        if (answer == 0)                   { step--; continue; }
        applyAndConfirm(car, step, answer - 1);
        step++;
    }
    return PartAssembleResult::Done;
}

int CarPartAssembler::showStep(int step) const {
    printf(CLEAR_SCREEN);
    const char* prompts[] = {
        "어떤 엔진을 탑재할까요?",
        "어떤 제동장치를 선택할까요?",
        "어떤 조향장치를 선택할까요?"
    };
    printf("%s\n", prompts[step]);
    printf("0. 뒤로가기\n");

    int count = printPartList(step);
    printf("===============================\n");
    return readInput(0, count);
}

int CarPartAssembler::printPartList(int step) const {
    int count = 0;
    auto print = [&](const std::string& n) {
        printf("%d. %s\n", ++count, n.c_str());
    };
    switch (step) {
    case 0: for (auto& p : engines_)          print(p->name()); break;
    case 1: for (auto& p : brakeSystems_)     print(p->name()); break;
    case 2: for (auto& p : steeringSystems_)  print(p->name()); break;
    }
    return count;
}

void CarPartAssembler::applyAndConfirm(Car& car, int step, int idx) const {
    std::string label;
    switch (step) {
    case 0: car.engine         = engines_[idx];
            label = car.engine->name()         + " 엔진";      break;
    case 1: car.brakeSystem    = brakeSystems_[idx];
            label = car.brakeSystem->name()    + " 제동장치";  break;
    case 2: car.steeringSystem = steeringSystems_[idx];
            label = car.steeringSystem->name() + " 조향장치";  break;
    }
    printf("%s을(를) 선택하셨습니다.\n", label.c_str());
    delayMs(800);
}
