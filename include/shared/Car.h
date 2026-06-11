#pragma once
#include <memory>
#include "../step1_type_selection/ICarType.h"
#include "../step2_part_assembly/IEngine.h"
#include "../step2_part_assembly/IBrakeSystem.h"
#include "../step2_part_assembly/ISteeringSystem.h"

class Car {
public:
    std::shared_ptr<ICarType>        type;
    std::shared_ptr<IEngine>         engine;
    std::shared_ptr<IBrakeSystem>    brakeSystem;
    std::shared_ptr<ISteeringSystem> steeringSystem;
};
