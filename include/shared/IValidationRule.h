#pragma once
#include <string>
#include "Car.h"

class IValidationRule {
public:
    virtual bool isSatisfied(const Car& car) const = 0;
    virtual std::string failReason(const Car& car) const = 0;
    virtual ~IValidationRule() = default;
};

class EngineCarTypeRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        return car.engine->isCompatibleWith(*car.type);
    }
    std::string failReason(const Car& car) const override {
        return car.type->name() + "에는 " + car.engine->name() + "엔진 사용 불가";
    }
};

class BrakeCarTypeRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        return car.brakeSystem->isCompatibleWith(*car.type);
    }
    std::string failReason(const Car& car) const override {
        return car.type->name() + "에는 " + car.brakeSystem->name() + "제동장치 사용 불가";
    }
};

class BoschBrakeSteeringRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        if (!car.brakeSystem->requiresBoschSteering()) return true;
        return car.steeringSystem->isBoschType();
    }
    std::string failReason(const Car& /*car*/) const override {
        return "Bosch제동장치에는 Bosch조향장치 이외 사용 불가";
    }
};
