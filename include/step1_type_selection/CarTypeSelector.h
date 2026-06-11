#pragma once
#include <vector>
#include <memory>
#include "ICarType.h"
#include "../shared/Car.h"

class CarTypeSelector {
public:
    using CarTypes = std::vector<std::shared_ptr<ICarType>>;

    explicit CarTypeSelector(CarTypes carTypes);
    bool select(Car& car);

private:
    CarTypes carTypes_;
};
