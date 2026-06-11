#include "../include/CarAssembler.h"

CarAssembler::CarAssembler(
    CarTypeSelector::CarTypes   carTypes,
    CarPartAssembler::Engines   engines,
    CarPartAssembler::Brakes    brakeSystems,
    CarPartAssembler::Steerings steeringSystems)
    : typeSelector_(std::move(carTypes))
    , partAssembler_(std::move(engines), std::move(brakeSystems), std::move(steeringSystems))
{}

bool CarAssembler::assemble(Car& car) {
    while (true) {
        if (!typeSelector_.select(car))              return false;  // exit 입력
        auto result = partAssembler_.assemble(car);
        if (result == PartAssembleResult::Done)      return true;   // 조립 완료
        if (result == PartAssembleResult::Exit)      return false;  // exit 입력
        // BackToTypeSelect → 루프 반복 (1단계부터 재시작)
    }
}
