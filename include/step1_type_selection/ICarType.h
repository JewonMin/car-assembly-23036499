#pragma once
#include <string>

class ICarType {
public:
    virtual std::string name() const = 0;
    virtual ~ICarType() = default;
};

class SedanType : public ICarType {
public: std::string name() const override { return "Sedan"; }
};
class SuvType : public ICarType {
public: std::string name() const override { return "SUV"; }
};
class TruckType : public ICarType {
public: std::string name() const override { return "Truck"; }
};
