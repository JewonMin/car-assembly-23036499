// ================================================================
// 공유 영역: 인터페이스·클래스 정의 (_DEBUG / Release 양쪽에서 사용)
// ================================================================
#ifdef UNIT_TESTING
#include "gmock/gmock.h"
#endif

#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>
#include <vector>
#include <memory>

constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";
constexpr int         EXIT_SIGNAL  = INT_MIN;

// ── 4-2: 차종 인터페이스 ──────────────────────────────────────────

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

// ── 4-1: 부품 인터페이스 ──────────────────────────────────────────

class IEngine {
public:
    virtual std::string name() const = 0;
    virtual bool isBroken() const { return false; }
    // 새 차종 추가 시 해당 엔진 클래스만 수정 (기존 CarValidator 수정 불필요)
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

// ── 4-3: Car 클래스 — 인터페이스 포인터 기반 ─────────────────────

class Car {
public:
    std::shared_ptr<ICarType>        type;
    std::shared_ptr<IEngine>         engine;
    std::shared_ptr<IBrakeSystem>    brakeSystem;
    std::shared_ptr<ISteeringSystem> steeringSystem;
};

// ── 4-4: 검증 규칙 인터페이스 ────────────────────────────────────

class IValidationRule {
public:
    virtual bool isSatisfied(const Car& car) const = 0;
    virtual std::string failReason(const Car& car) const = 0;
    virtual ~IValidationRule() = default;
};

// 엔진-차종 호환성: Toyota/SUV 불가, WIA/Truck 불가
class EngineCarTypeRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        return car.engine->isCompatibleWith(*car.type);
    }
    std::string failReason(const Car& car) const override {
        return car.type->name() + "에는 " + car.engine->name() + "엔진 사용 불가";
    }
};

// 제동장치-차종 호환성: Continental/Sedan 불가, Mando/Truck 불가
class BrakeCarTypeRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        return car.brakeSystem->isCompatibleWith(*car.type);
    }
    std::string failReason(const Car& car) const override {
        return car.type->name() + "에는 " + car.brakeSystem->name() + "제동장치 사용 불가";
    }
};

// Bosch 제동장치 사용 시 Bosch 조향장치 필수
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

// ── CarValidator — 규칙 목록 기반, 새 규칙 추가 시 이 클래스 수정 불필요 ──

class CarValidator {
public:
    void addRule(std::shared_ptr<IValidationRule> rule) {
        rules_.push_back(rule);
    }
    bool isValid(const Car& car) const {
        for (const auto& rule : rules_)
            if (!rule->isSatisfied(car)) return false;
        return true;
    }
    std::string getFailReason(const Car& car) const {
        for (const auto& rule : rules_)
            if (!rule->isSatisfied(car)) return rule->failReason(car);
        return "";
    }
private:
    std::vector<std::shared_ptr<IValidationRule>> rules_;
};

// PDF p.23 제한조건 5가지를 규칙 3개(EngineCarType + BrakeCarType + BoschSteering)로 표현
inline CarValidator makeDefaultValidator() {
    CarValidator v;
    v.addRule(std::make_shared<EngineCarTypeRule>());
    v.addRule(std::make_shared<BrakeCarTypeRule>());
    v.addRule(std::make_shared<BoschBrakeSteeringRule>());
    return v;
}

// ================================================================
// #ifdef UNIT_TESTING: 유닛 테스트 (4-6)
// #else              : 실행 코드 (4-5, main)
// ================================================================
#ifdef UNIT_TESTING

using ::testing::Test;

// ── CarValidatorTest: 기본 규칙 세트 전체 검증 ──

TEST(CarValidatorTest, Valid_Sedan_GM_Mando_Bosch)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_TRUE(v.isValid(car));
}

TEST(CarValidatorTest, Invalid_Sedan_Continental)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<ContinentalBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(v.isValid(car));
    EXPECT_EQ(v.getFailReason(car), "Sedan에는 Continental제동장치 사용 불가");
}

TEST(CarValidatorTest, Invalid_SUV_Toyota)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<SuvType>(), std::make_shared<ToyotaEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(v.isValid(car));
    EXPECT_EQ(v.getFailReason(car), "SUV에는 TOYOTA엔진 사용 불가");
}

TEST(CarValidatorTest, Invalid_Truck_WIA)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<TruckType>(), std::make_shared<WiaEngine>(),
             std::make_shared<BoschBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(v.isValid(car));
    EXPECT_EQ(v.getFailReason(car), "Truck에는 WIA엔진 사용 불가");
}

TEST(CarValidatorTest, Invalid_Truck_Mando)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<TruckType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(v.isValid(car));
    EXPECT_EQ(v.getFailReason(car), "Truck에는 Mando제동장치 사용 불가");
}

TEST(CarValidatorTest, Invalid_BoschBrake_MobisSteering)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<BoschBrake>(), std::make_shared<MobisSteering>() };
    EXPECT_FALSE(v.isValid(car));
    EXPECT_EQ(v.getFailReason(car), "Bosch제동장치에는 Bosch조향장치 이외 사용 불가");
}

TEST(CarValidatorTest, Valid_BoschBrake_BoschSteering)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<BoschBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_TRUE(v.isValid(car));
}

// ── 개별 규칙 테스트 ──

TEST(EngineCarTypeRuleTest, Toyota_Invalid_For_SUV)
{
    EngineCarTypeRule rule;
    Car car{ std::make_shared<SuvType>(), std::make_shared<ToyotaEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(rule.isSatisfied(car));
}

TEST(EngineCarTypeRuleTest, WIA_Invalid_For_Truck)
{
    EngineCarTypeRule rule;
    Car car{ std::make_shared<TruckType>(), std::make_shared<WiaEngine>(),
             std::make_shared<BoschBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(rule.isSatisfied(car));
}

TEST(BrakeCarTypeRuleTest, Continental_Invalid_For_Sedan)
{
    BrakeCarTypeRule rule;
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<ContinentalBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(rule.isSatisfied(car));
}

TEST(BrakeCarTypeRuleTest, Mando_Invalid_For_Truck)
{
    BrakeCarTypeRule rule;
    Car car{ std::make_shared<TruckType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_FALSE(rule.isSatisfied(car));
}

TEST(BoschBrakeSteeringRuleTest, BoschBrake_Requires_BoschSteering)
{
    BoschBrakeSteeringRule rule;
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<BoschBrake>(), std::make_shared<MobisSteering>() };
    EXPECT_FALSE(rule.isSatisfied(car));
}

TEST(BoschBrakeSteeringRuleTest, NonBoschBrake_DoesNotRequireBoschSteering)
{
    BoschBrakeSteeringRule rule;
    Car car{ std::make_shared<SedanType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<MobisSteering>() };
    EXPECT_TRUE(rule.isSatisfied(car));
}

// ── 확장성 테스트: 새 차종 추가 시 기존 코드 수정 없이 동작 확인 ──

class VanType : public ICarType {
public: std::string name() const override { return "Van"; }
};

TEST(CarValidatorTest, NewVanType_Works_Without_Modifying_Existing_Code)
{
    auto v = makeDefaultValidator();
    // Van + GM + Mando + Bosch: 기존 제한조건에 해당 없음 → PASS
    Car car{ std::make_shared<VanType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_TRUE(v.isValid(car));
}

int main()
{
    testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}

#else

// ── 유틸리티 함수 ──────────────────────────────────────────────

void delayMs(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int readInput(int minVal, int maxVal)
{
    std::string input;
    while (true)
    {
        printf("INPUT > ");
        std::getline(std::cin, input);
        if (input == "exit") return EXIT_SIGNAL;

        int  answer = 0;
        bool isNum  = false;
        try { answer = std::stoi(input); isNum = true; } catch (...) {}

        if (!isNum)
            printf("ERROR :: 숫자만 입력 가능\n");
        else if (answer < minVal || answer > maxVal)
            printf("ERROR :: %d ~ %d 범위만 선택 가능\n", minVal, maxVal);
        else
            return answer;

        delayMs(800);
    }
}

// 4-5: CarAssembler — 부품 목록을 외부에서 주입받아 메뉴 자동 구성
class CarAssembler {
public:
    using CarTypes   = std::vector<std::shared_ptr<ICarType>>;
    using Engines    = std::vector<std::shared_ptr<IEngine>>;
    using Brakes     = std::vector<std::shared_ptr<IBrakeSystem>>;
    using Steerings  = std::vector<std::shared_ptr<ISteeringSystem>>;

    CarAssembler(CarTypes t, Engines e, Brakes b, Steerings s)
        : carTypes_(t), engines_(e), brakeSystems_(b), steeringSystems_(s) {}

    bool assemble(Car& car)
    {
        int step = 0;
        while (step < 4)
        {
            int answer = showStep(step);
            if (answer == EXIT_SIGNAL) return false;
            if (answer == 0) { step--; continue; }

            applyAndConfirm(car, step, answer - 1);
            step++;
        }
        return true;
    }

private:
    CarTypes  carTypes_;
    Engines   engines_;
    Brakes    brakeSystems_;
    Steerings steeringSystems_;

    int showStep(int step) const
    {
        printf(CLEAR_SCREEN);
        if (step == 0)
        {
            printf("        ______________\n");
            printf("       /|            | \n");
            printf("  ____/_|_____________|____\n");
            printf(" |                      O  |\n");
            printf(" '-(@)----------------(@)--'\n");
            printf("===============================\n");
        }

        const char* prompts[] = {
            "어떤 차량 타입을 선택할까요?",
            "어떤 엔진을 탑재할까요?",
            "어떤 제동장치를 선택할까요?",
            "어떤 조향장치를 선택할까요?"
        };
        printf("%s\n", prompts[step]);
        if (step > 0) printf("0. 뒤로가기\n");

        int count = printPartList(step);
        printf("===============================\n");
        return readInput(step == 0 ? 1 : 0, count);
    }

    int printPartList(int step) const
    {
        int count = 0;
        auto print = [&](const std::string& n) {
            printf("%d. %s\n", ++count, n.c_str());
        };
        switch (step)
        {
        case 0: for (auto& p : carTypes_)       print(p->name()); break;
        case 1: for (auto& p : engines_)        print(p->name()); break;
        case 2: for (auto& p : brakeSystems_)   print(p->name()); break;
        case 3: for (auto& p : steeringSystems_) print(p->name()); break;
        }
        return count;
    }

    void applyAndConfirm(Car& car, int step, int idx) const
    {
        std::string label;
        switch (step)
        {
        case 0: car.type          = carTypes_[idx];
                label = car.type->name()          + " 차량 타입"; break;
        case 1: car.engine        = engines_[idx];
                label = car.engine->name()        + " 엔진";     break;
        case 2: car.brakeSystem   = brakeSystems_[idx];
                label = car.brakeSystem->name()   + " 제동장치"; break;
        case 3: car.steeringSystem = steeringSystems_[idx];
                label = car.steeringSystem->name() + " 조향장치"; break;
        }
        printf("%s을(를) 선택하셨습니다.\n", label.c_str());
        delayMs(800);
    }
};

// ── 4-4: UI 표시 함수 — name() 메서드로 동적 출력, if-else 체인 불필요 ──

void runCar(const Car& car, const CarValidator& validator)
{
    if (!validator.isValid(car)) {
        printf("자동차가 동작되지 않습니다\n");
        return;
    }
    if (car.engine->isBroken()) {
        printf("엔진이 고장나있습니다.\n자동차가 움직이지 않습니다.\n");
        return;
    }
    printf("Car Type : %s\n",       car.type->name().c_str());
    printf("Engine : %s\n",         car.engine->name().c_str());
    printf("Brake System : %s\n",   car.brakeSystem->name().c_str());
    printf("SteeringSystem : %s\n", car.steeringSystem->name().c_str());
    printf("자동차가 동작됩니다.\n");
}

void testCar(const Car& car, const CarValidator& validator)
{
    if (validator.isValid(car)) {
        printf("자동차 부품 조합 테스트 결과 : PASS\n");
        return;
    }
    printf("자동차 부품 조합 테스트 결과 : FAIL\n");
    printf("%s\n", validator.getFailReason(car).c_str());
}

// main — CarAssembler와 CarValidator에 부품·규칙을 주입해 조립
int main()
{
    CarAssembler assembler(
        { std::make_shared<SedanType>(),  std::make_shared<SuvType>(),  std::make_shared<TruckType>() },
        { std::make_shared<GmEngine>(),   std::make_shared<ToyotaEngine>(),
          std::make_shared<WiaEngine>(),  std::make_shared<BrokenEngine>() },
        { std::make_shared<MandoBrake>(), std::make_shared<ContinentalBrake>(), std::make_shared<BoschBrake>() },
        { std::make_shared<BoschSteering>(), std::make_shared<MobisSteering>() }
    );
    CarValidator validator = makeDefaultValidator();
    Car car;

    while (true)
    {
        if (!assembler.assemble(car)) { printf("바이바이\n"); break; }

        while (true)
        {
            printf(CLEAR_SCREEN);
            printf("멋진 차량이 완성되었습니다.\n어떤 동작을 할까요?\n");
            printf("0. 처음 화면으로 돌아가기\n1. RUN\n2. Test\n");
            printf("===============================\n");

            int answer = readInput(0, 2);
            if (answer == EXIT_SIGNAL) { printf("바이바이\n"); return 0; }
            if (answer == 0) break;
            if (answer == 1) { runCar(car, validator);  delayMs(2000); }
            if (answer == 2) { printf("Test...\n"); delayMs(1500); testCar(car, validator); delayMs(2000); }
        }
    }
    return 0;
}

#endif
