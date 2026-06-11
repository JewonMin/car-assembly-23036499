#pragma once
#include <vector>
#include <memory>
#include "IValidationRule.h"

class CarValidator {
public:
    void addRule(std::shared_ptr<IValidationRule> rule);
    bool isValid(const Car& car) const;
    std::string getFailReason(const Car& car) const;

private:
    std::vector<std::shared_ptr<IValidationRule>> rules_;
};

CarValidator makeDefaultValidator();
