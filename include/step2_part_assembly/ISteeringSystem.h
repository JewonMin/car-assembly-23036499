#pragma once
#include <string>

class ISteeringSystem {
public:
    virtual std::string name() const = 0;
    virtual bool isBoschType() const { return false; }
    virtual ~ISteeringSystem() = default;
};

class BoschSteering : public ISteeringSystem {
public:
    std::string name() const override { return "Bosch"; }
    bool isBoschType() const override { return true; }
};
class MobisSteering : public ISteeringSystem {
public: std::string name() const override { return "Mobis"; }
};
