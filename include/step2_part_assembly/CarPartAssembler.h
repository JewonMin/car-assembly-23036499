#pragma once
#include <vector>
#include <memory>
#include "IEngine.h"
#include "IBrakeSystem.h"
#include "ISteeringSystem.h"
#include "../shared/Car.h"

enum class PartAssembleResult { Done, BackToTypeSelect, Exit };

class CarPartAssembler {
public:
    using Engines   = std::vector<std::shared_ptr<IEngine>>;
    using Brakes    = std::vector<std::shared_ptr<IBrakeSystem>>;
    using Steerings = std::vector<std::shared_ptr<ISteeringSystem>>;

    CarPartAssembler(Engines e, Brakes b, Steerings s);
    PartAssembleResult assemble(Car& car);

private:
    Engines   engines_;
    Brakes    brakeSystems_;
    Steerings steeringSystems_;

    int  showStep(int step) const;
    int  printPartList(int step) const;
    void applyAndConfirm(Car& car, int step, int idx) const;
};
