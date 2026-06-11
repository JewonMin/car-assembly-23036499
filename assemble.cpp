// ================================================================
// 공유 영역: 열거형·클래스 정의 (_DEBUG / Release 양쪽에서 사용)
// ================================================================
#ifdef _DEBUG
#include "gmock/gmock.h"
#endif

#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>

constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";
constexpr int         EXIT_SIGNAL  = INT_MIN;

enum CarType        { SEDAN = 1, SUV, TRUCK };
enum Engine         { GM = 1, TOYOTA, WIA, BROKEN };
enum BrakeSystem    { MANDO = 1, CONTINENTAL, BOSCH_B };
enum SteeringSystem { BOSCH_S = 1, MOBIS };

// 3-1: Car 클래스 — 조립 완성 차량의 구성 데이터 보유
class Car {
public:
    CarType        type;
    Engine         engine;
    BrakeSystem    brakeSystem;
    SteeringSystem steeringSystem;
};

// 3-2: CarValidator — 순수 비즈니스 로직 (UI·전역 상태 없음, 독립 테스트 가능)
class CarValidator {
public:
    bool isValid(const Car& car) const {
        return checkTypePartCompatibility(car) && checkBoschCompatibility(car);
    }

    std::string getFailReason(const Car& car) const {
        if (car.type == SEDAN && car.brakeSystem == CONTINENTAL)
            return "Sedan에는 Continental제동장치 사용 불가";
        if (car.type == SUV   && car.engine      == TOYOTA)
            return "SUV에는 TOYOTA엔진 사용 불가";
        if (car.type == TRUCK && car.engine      == WIA)
            return "Truck에는 WIA엔진 사용 불가";
        if (car.type == TRUCK && car.brakeSystem == MANDO)
            return "Truck에는 Mando제동장치 사용 불가";
        if (car.brakeSystem   == BOSCH_B && car.steeringSystem != BOSCH_S)
            return "Bosch제동장치에는 Bosch조향장치 이외 사용 불가";
        return "";
    }

private:
    bool checkBoschCompatibility(const Car& car) const {
        return !(car.brakeSystem == BOSCH_B && car.steeringSystem != BOSCH_S);
    }

    bool checkTypePartCompatibility(const Car& car) const {
        if (car.type == SEDAN && car.brakeSystem == CONTINENTAL) return false;
        if (car.type == SUV   && car.engine      == TOYOTA)      return false;
        if (car.type == TRUCK && car.engine      == WIA)         return false;
        if (car.type == TRUCK && car.brakeSystem == MANDO)       return false;
        return true;
    }
};

// ================================================================
// #ifdef _DEBUG: 유닛 테스트 (3-5)
// #else        : 실행 코드 (3-3, 3-4, main)
// ================================================================
#ifdef _DEBUG

// 3-5: CarValidator 유닛 테스트 — 제한조건 5가지 + 유효 조합 검증
TEST(CarValidatorTest, Valid_Sedan_GM_MANDO_BOSCH_S)
{
    CarValidator v;
    EXPECT_TRUE(v.isValid({SEDAN, GM, MANDO, BOSCH_S}));
}

TEST(CarValidatorTest, Invalid_Sedan_Continental)
{
    CarValidator v;
    EXPECT_FALSE(v.isValid({SEDAN, GM, CONTINENTAL, BOSCH_S}));
}

TEST(CarValidatorTest, Invalid_SUV_Toyota)
{
    CarValidator v;
    EXPECT_FALSE(v.isValid({SUV, TOYOTA, MANDO, BOSCH_S}));
}

TEST(CarValidatorTest, Invalid_Truck_WIA)
{
    CarValidator v;
    EXPECT_FALSE(v.isValid({TRUCK, WIA, MANDO, BOSCH_S}));
}

TEST(CarValidatorTest, Invalid_Truck_MANDO)
{
    CarValidator v;
    EXPECT_FALSE(v.isValid({TRUCK, GM, MANDO, BOSCH_S}));
}

TEST(CarValidatorTest, Invalid_BOSCH_B_NonBoschSteering)
{
    CarValidator v;
    EXPECT_FALSE(v.isValid({SEDAN, GM, BOSCH_B, MOBIS}));
}

TEST(CarValidatorTest, Valid_BOSCH_B_BoschSteering)
{
    CarValidator v;
    EXPECT_TRUE(v.isValid({SEDAN, GM, BOSCH_B, BOSCH_S}));
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

// 유효한 범위의 숫자가 입력될 때까지 반복. "exit" → EXIT_SIGNAL 반환
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

// 3-3: CarAssembler — 조립 워크플로우 (메뉴 출력 + 부품 선택 + 뒤로가기)
class CarAssembler {
public:
    // false 반환 = 사용자가 "exit" 입력
    bool assemble(Car& car)
    {
        const int maxInput[] = {3, 4, 3, 2};  // 단계별 최대 입력값
        int step = CAR_TYPE;

        while (step <= STEERING_SYSTEM)
        {
            printMenu(static_cast<Step>(step));
            int minVal = (step == CAR_TYPE) ? 1 : 0;
            int answer = readInput(minVal, maxInput[step]);

            if (answer == EXIT_SIGNAL) return false;
            if (answer == 0) { step--; continue; }  // 뒤로가기

            printConfirm(static_cast<Step>(step), answer);
            delayMs(800);

            switch (step)
            {
            case CAR_TYPE:        car.type          = static_cast<CarType>(answer);        break;
            case ENGINE:          car.engine         = static_cast<Engine>(answer);         break;
            case BRAKE_SYSTEM:    car.brakeSystem    = static_cast<BrakeSystem>(answer);    break;
            case STEERING_SYSTEM: car.steeringSystem = static_cast<SteeringSystem>(answer); break;
            }
            step++;
        }
        return true;
    }

private:
    enum Step { CAR_TYPE = 0, ENGINE, BRAKE_SYSTEM, STEERING_SYSTEM };

    void printMenu(Step step) const
    {
        printf(CLEAR_SCREEN);
        switch (step)
        {
        case CAR_TYPE:
            printf("        ______________\n");
            printf("       /|            | \n");
            printf("  ____/_|_____________|____\n");
            printf(" |                      O  |\n");
            printf(" '-(@)----------------(@)--'\n");
            printf("===============================\n");
            printf("어떤 차량 타입을 선택할까요?\n");
            printf("1. Sedan\n2. SUV\n3. Truck\n");
            break;
        case ENGINE:
            printf("어떤 엔진을 탑재할까요?\n");
            printf("0. 뒤로가기\n1. GM\n2. TOYOTA\n3. WIA\n4. 고장난 엔진\n");
            break;
        case BRAKE_SYSTEM:
            printf("어떤 제동장치를 선택할까요?\n");
            printf("0. 뒤로가기\n1. MANDO\n2. CONTINENTAL\n3. BOSCH\n");
            break;
        case STEERING_SYSTEM:
            printf("어떤 조향장치를 선택할까요?\n");
            printf("0. 뒤로가기\n1. BOSCH\n2. MOBIS\n");
            break;
        }
        printf("===============================\n");
    }

    void printConfirm(Step step, int answer) const
    {
        switch (step)
        {
        case CAR_TYPE:
            if      (answer == SEDAN)  printf("차량 타입으로 Sedan을 선택하셨습니다.\n");
            else if (answer == SUV)    printf("차량 타입으로 SUV을 선택하셨습니다.\n");
            else if (answer == TRUCK)  printf("차량 타입으로 Truck을 선택하셨습니다.\n");
            break;
        case ENGINE:
            if      (answer == GM)     printf("GM 엔진을 선택하셨습니다.\n");
            else if (answer == TOYOTA) printf("TOYOTA 엔진을 선택하셨습니다.\n");
            else if (answer == WIA)    printf("WIA 엔진을 선택하셨습니다.\n");
            else if (answer == BROKEN) printf("고장난 엔진을 선택하셨습니다.\n");
            break;
        case BRAKE_SYSTEM:
            if      (answer == MANDO)       printf("MANDO 제동장치를 선택하셨습니다.\n");
            else if (answer == CONTINENTAL) printf("CONTINENTAL 제동장치를 선택하셨습니다.\n");
            else if (answer == BOSCH_B)     printf("BOSCH 제동장치를 선택하셨습니다.\n");
            break;
        case STEERING_SYSTEM:
            if      (answer == BOSCH_S) printf("BOSCH 조향장치를 선택하셨습니다.\n");
            else if (answer == MOBIS)   printf("MOBIS 조향장치를 선택하셨습니다.\n");
            break;
        }
    }
};

// 3-4: UI 표시 함수 — 비즈니스 로직(CarValidator)과 출력 책임 분리
void runCar(const Car& car, const CarValidator& validator)
{
    if (!validator.isValid(car))
    {
        printf("자동차가 동작되지 않습니다\n");
        return;
    }
    if (car.engine == BROKEN)
    {
        printf("엔진이 고장나있습니다.\n");
        printf("자동차가 움직이지 않습니다.\n");
        return;
    }

    if      (car.type == SEDAN) printf("Car Type : Sedan\n");
    else if (car.type == SUV)   printf("Car Type : SUV\n");
    else if (car.type == TRUCK) printf("Car Type : Truck\n");

    if      (car.engine == GM)     printf("Engine : GM\n");
    else if (car.engine == TOYOTA) printf("Engine : TOYOTA\n");
    else if (car.engine == WIA)    printf("Engine : WIA\n");

    if      (car.brakeSystem == MANDO)       printf("Brake System : Mando\n");
    else if (car.brakeSystem == CONTINENTAL) printf("Brake System : Continental\n");
    else if (car.brakeSystem == BOSCH_B)     printf("Brake System : Bosch\n");

    if      (car.steeringSystem == BOSCH_S) printf("SteeringSystem : Bosch\n");
    else if (car.steeringSystem == MOBIS)   printf("SteeringSystem : Mobis\n");

    printf("자동차가 동작됩니다.\n");
}

void testCar(const Car& car, const CarValidator& validator)
{
    if (validator.isValid(car))
    {
        printf("자동차 부품 조합 테스트 결과 : PASS\n");
        return;
    }
    printf("자동차 부품 조합 테스트 결과 : FAIL\n");
    printf("%s\n", validator.getFailReason(car).c_str());
}

// 3-4: main — UI 루프만 담당, 비즈니스 로직은 CarValidator에 위임
int main()
{
    CarAssembler assembler;
    CarValidator  validator;
    Car           car;

    while (true)
    {
        if (!assembler.assemble(car))
        {
            printf("바이바이\n");
            break;
        }

        while (true)
        {
            printf(CLEAR_SCREEN);
            printf("멋진 차량이 완성되었습니다.\n");
            printf("어떤 동작을 할까요?\n");
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
