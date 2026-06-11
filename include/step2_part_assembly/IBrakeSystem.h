#pragma once
#include <string>
#include "../step1_type_selection/ICarType.h"

class IBrakeSystem {
public:
    virtual std::string name() const = 0;
    virtual bool isCompatibleWith(const ICarType&) const { return true; }
    virtual bool requiresBoschSteering() const { return false; }
    virtual ~IBrakeSystem() = default;
};

class MandoBrake : public IBrakeSystem {
public:
    std::string name() const override { return "Mando"; }
    bool isCompatibleWith(const ICarType& t) const override {
        return dynamic_cast<const TruckType*>(&t) == nullptr;
    }
};
class ContinentalBrake : public IBrakeSystem {
public:
    std::string name() const override { return "Continental"; }
    bool isCompatibleWith(const ICarType& t) const override {
        return dynamic_cast<const SedanType*>(&t) == nullptr;
    }
};
class BoschBrake : public IBrakeSystem {
public:
    std::string name() const override { return "Bosch"; }
    bool requiresBoschSteering() const override { return true; }
};
