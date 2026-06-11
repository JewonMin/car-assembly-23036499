#pragma once
#include "step1_type_selection/CarTypeSelector.h"
#include "step2_part_assembly/CarPartAssembler.h"

class CarAssembler {
public:
    CarAssembler(
        CarTypeSelector::CarTypes   carTypes,
        CarPartAssembler::Engines   engines,
        CarPartAssembler::Brakes    brakeSystems,
        CarPartAssembler::Steerings steeringSystems
    );

    // 1단계(차종 선택) → 2단계(부품 조립) 순서로 진행
    // false = 사용자가 종료(exit 입력)
    bool assemble(Car& car);

private:
    CarTypeSelector  typeSelector_;
    CarPartAssembler partAssembler_;
};
