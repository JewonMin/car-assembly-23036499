#include "gmock/gmock.h"
#include "../include/shared/CarValidator.h"
#include "../include/step1_type_selection/ICarType.h"
#include "../include/step2_part_assembly/IEngine.h"
#include "../include/step2_part_assembly/IBrakeSystem.h"
#include "../include/step2_part_assembly/ISteeringSystem.h"

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

// ── 확장성 테스트: 기존 코드 수정 없이 신규 차종 추가 ──

class VanType : public ICarType {
public: std::string name() const override { return "Van"; }
};

TEST(CarValidatorTest, NewVanType_Works_Without_Modifying_Existing_Code)
{
    auto v = makeDefaultValidator();
    Car car{ std::make_shared<VanType>(), std::make_shared<GmEngine>(),
             std::make_shared<MandoBrake>(), std::make_shared<BoschSteering>() };
    EXPECT_TRUE(v.isValid(car));
}

int main() {
    testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}
