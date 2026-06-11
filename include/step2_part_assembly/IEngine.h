#pragma once
#include <string>
#include "../step1_type_selection/ICarType.h"

class IEngine {
public:
    virtual std::string name() const = 0;
    virtual bool isBroken() const { return false; }
    virtual bool isCompatibleWith(const ICarType&) const { return true; }
    virtual ~IEngine() = default;
};

class GmEngine : public IEngine {
public: std::string name() const override { return "GM"; }
};
class ToyotaEngine : public IEngine {
public:
    std::string name() const override { return "TOYOTA"; }
    bool isCompatibleWith(const ICarType& t) const override {
        return dynamic_cast<const SuvType*>(&t) == nullptr;
    }
};
class WiaEngine : public IEngine {
public:
    std::string name() const override { return "WIA"; }
    bool isCompatibleWith(const ICarType& t) const override {
        return dynamic_cast<const TruckType*>(&t) == nullptr;
    }
};
class BrokenEngine : public IEngine {
public:
    std::string name() const override { return "고장난 엔진"; }
    bool isBroken() const override { return true; }
};
