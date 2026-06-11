#include "../../include/shared/CarValidator.h"

void CarValidator::addRule(std::shared_ptr<IValidationRule> rule) {
    rules_.push_back(rule);
}

bool CarValidator::isValid(const Car& car) const {
    for (const auto& rule : rules_)
        if (!rule->isSatisfied(car)) return false;
    return true;
}

std::string CarValidator::getFailReason(const Car& car) const {
    for (const auto& rule : rules_)
        if (!rule->isSatisfied(car)) return rule->failReason(car);
    return "";
}

CarValidator makeDefaultValidator() {
    CarValidator v;
    v.addRule(std::make_shared<EngineCarTypeRule>());
    v.addRule(std::make_shared<BrakeCarTypeRule>());
    v.addRule(std::make_shared<BoschBrakeSteeringRule>());
    return v;
}
