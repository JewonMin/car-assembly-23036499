#ifdef _DEBUG

#include "gmock/gmock.h"

int main()
{
    testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}

#else

#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";

// 2-2: UI 진행 단계만 담당 — 더 이상 배열 인덱스로 사용하지 않음
enum QuestionType
{
    CarType_Q,
    Engine_Q,
    BrakeSystem_Q,
    SteeringSystem_Q,
    Run_Test,
};

enum CarType
{
    SEDAN = 1,
    SUV,
    TRUCK
};

enum Engine
{
    GM = 1,
    TOYOTA,
    WIA,
    BROKEN
};

enum BrakeSystem
{
    MANDO = 1,
    CONTINENTAL,
    BOSCH_B
};

enum SteeringSystem
{
    BOSCH_S = 1,
    MOBIS
};

// 2-1: 전역 int stack[10] 제거 → 타입이 명확한 구조체로 교체
struct CarConfig
{
    CarType        carType;
    Engine         engine;
    BrakeSystem    brakeSystem;
    SteeringSystem steeringSystem;
};

void selectCarType(CarConfig& config, int answer);
void selectEngine(CarConfig& config, int answer);
void selectBrakeSystem(CarConfig& config, int answer);
void selectSteeringSystem(CarConfig& config, int answer);
bool isValidCheck(const CarConfig& config);  // 2-3: 전역 상태 의존 제거
void runProducedCar(const CarConfig& config);
void testProducedCar(const CarConfig& config);
void delay(int ms);

void delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main()
{
    std::string input;
    int step = CarType_Q;
    CarConfig config{};  // 2-1: 전역 stack[] 대신 지역 변수로 관리

    while (1)
    {
        if (step == CarType_Q)
        {
            printf(CLEAR_SCREEN);
            printf("        ______________\n");
            printf("       /|            | \n");
            printf("  ____/_|_____________|____\n");
            printf(" |                      O  |\n");
            printf(" '-(@)----------------(@)--'\n");
            printf("===============================\n");
            printf("어떤 차량 타입을 선택할까요?\n");
            printf("1. Sedan\n");
            printf("2. SUV\n");
            printf("3. Truck\n");
        }
        else if (step == Engine_Q)
        {
            printf(CLEAR_SCREEN);
            printf("어떤 엔진을 탑재할까요?\n");
            printf("0. 뒤로가기\n");
            printf("1. GM\n");
            printf("2. TOYOTA\n");
            printf("3. WIA\n");
            printf("4. 고장난 엔진\n");
        }
        else if (step == BrakeSystem_Q)
        {
            printf(CLEAR_SCREEN);
            printf("어떤 제동장치를 선택할까요?\n");
            printf("0. 뒤로가기\n");
            printf("1. MANDO\n");
            printf("2. CONTINENTAL\n");
            printf("3. BOSCH\n");
        }
        else if (step == SteeringSystem_Q)
        {
            printf(CLEAR_SCREEN);
            printf("어떤 조향장치를 선택할까요?\n");
            printf("0. 뒤로가기\n");
            printf("1. BOSCH\n");
            printf("2. MOBIS\n");
        }
        else if (step == Run_Test)
        {
            printf(CLEAR_SCREEN);
            printf("멋진 차량이 완성되었습니다.\n");
            printf("어떤 동작을 할까요?\n");
            printf("0. 처음 화면으로 돌아가기\n");
            printf("1. RUN\n");
            printf("2. Test\n");
        }
        printf("===============================\n");
        printf("INPUT > ");

        std::getline(std::cin, input);

        if (input == "exit")
        {
            printf("바이바이\n");
            break;
        }

        int answer = 0;
        try
        {
            answer = std::stoi(input);
        }
        catch (...)
        {
            printf("ERROR :: 숫자만 입력 가능\n");
            delay(800);
            continue;
        }

        if (step == CarType_Q && !(answer >= 1 && answer <= 3))
        {
            printf("ERROR :: 차량 타입은 1 ~ 3 범위만 선택 가능\n");
            delay(800);
            continue;
        }

        if (step == Engine_Q && !(answer >= 0 && answer <= 4))
        {
            printf("ERROR :: 엔진은 1 ~ 4 범위만 선택 가능\n");
            delay(800);
            continue;
        }

        if (step == BrakeSystem_Q && !(answer >= 0 && answer <= 3))
        {
            printf("ERROR :: 제동장치는 1 ~ 3 범위만 선택 가능\n");
            delay(800);
            continue;
        }

        if (step == SteeringSystem_Q && !(answer >= 0 && answer <= 2))
        {
            printf("ERROR :: 조향장치는 1 ~ 2 범위만 선택 가능\n");
            delay(800);
            continue;
        }

        if (step == Run_Test && !(answer >= 0 && answer <= 2))
        {
            printf("ERROR :: Run 또는 Test 중 하나를 선택 필요\n");
            delay(800);
            continue;
        }

        if (answer == 0 && step == Run_Test)
        {
            step = CarType_Q;
            continue;
        }

        if (answer == 0 && step >= 1)
        {
            step -= 1;
            continue;
        }

        if (step == CarType_Q)
        {
            selectCarType(config, answer);
            delay(800);
            step = Engine_Q;
        }
        else if (step == Engine_Q)
        {
            selectEngine(config, answer);
            delay(800);
            step = BrakeSystem_Q;
        }
        else if (step == BrakeSystem_Q)
        {
            selectBrakeSystem(config, answer);
            delay(800);
            step = SteeringSystem_Q;
        }
        else if (step == SteeringSystem_Q)
        {
            selectSteeringSystem(config, answer);
            delay(800);
            step = Run_Test;
        }
        else if (step == Run_Test && answer == 1)
        {
            runProducedCar(config);
            delay(2000);
        }
        else if (step == Run_Test && answer == 2)
        {
            printf("Test...\n");
            delay(1500);
            testProducedCar(config);
            delay(2000);
        }
    }
}

void selectCarType(CarConfig& config, int answer)
{
    config.carType = static_cast<CarType>(answer);
    if (answer == SEDAN)
        printf("차량 타입으로 Sedan을 선택하셨습니다.\n");
    else if (answer == SUV)
        printf("차량 타입으로 SUV을 선택하셨습니다.\n");
    else if (answer == TRUCK)
        printf("차량 타입으로 Truck을 선택하셨습니다.\n");
}

void selectEngine(CarConfig& config, int answer)
{
    config.engine = static_cast<Engine>(answer);
    if (answer == GM)
        printf("GM 엔진을 선택하셨습니다.\n");
    else if (answer == TOYOTA)
        printf("TOYOTA 엔진을 선택하셨습니다.\n");
    else if (answer == WIA)
        printf("WIA 엔진을 선택하셨습니다.\n");
    else if (answer == BROKEN)
        printf("고장난 엔진을 선택하셨습니다.\n");
}

void selectBrakeSystem(CarConfig& config, int answer)
{
    config.brakeSystem = static_cast<BrakeSystem>(answer);
    if (answer == MANDO)
        printf("MANDO 제동장치를 선택하셨습니다.\n");
    else if (answer == CONTINENTAL)
        printf("CONTINENTAL 제동장치를 선택하셨습니다.\n");
    else if (answer == BOSCH_B)
        printf("BOSCH 제동장치를 선택하셨습니다.\n");
}

void selectSteeringSystem(CarConfig& config, int answer)
{
    config.steeringSystem = static_cast<SteeringSystem>(answer);
    if (answer == BOSCH_S)
        printf("BOSCH 조향장치를 선택하셨습니다.\n");
    else if (answer == MOBIS)
        printf("MOBIS 조향장치를 선택하셨습니다.\n");
}

// 2-3: 검증 단일 진입점 — 전역 상태 대신 CarConfig를 인자로 받아 어디서든 호출 가능
bool isValidCheck(const CarConfig& config)
{
    if (config.carType == SEDAN && config.brakeSystem == CONTINENTAL)
        return false;
    else if (config.carType == SUV && config.engine == TOYOTA)
        return false;
    else if (config.carType == TRUCK && config.engine == WIA)
        return false;
    else if (config.carType == TRUCK && config.brakeSystem == MANDO)
        return false;
    else if (config.brakeSystem == BOSCH_B && config.steeringSystem != BOSCH_S)
        return false;
    return true;
}

void runProducedCar(const CarConfig& config)
{
    if (!isValidCheck(config))
    {
        printf("자동차가 동작되지 않습니다\n");
        return;
    }

    if (config.engine == BROKEN)
    {
        printf("엔진이 고장나있습니다.\n");
        printf("자동차가 움직이지 않습니다.\n");
        return;
    }

    if (config.carType == SEDAN)
        printf("Car Type : Sedan\n");
    else if (config.carType == SUV)
        printf("Car Type : SUV\n");
    else if (config.carType == TRUCK)
        printf("Car Type : Truck\n");

    if (config.engine == GM)
        printf("Engine : GM\n");
    else if (config.engine == TOYOTA)
        printf("Engine : TOYOTA\n");
    else if (config.engine == WIA)
        printf("Engine : WIA\n");

    if (config.brakeSystem == MANDO)
        printf("Brake System : Mando\n");
    else if (config.brakeSystem == CONTINENTAL)
        printf("Brake System : Continental\n");
    else if (config.brakeSystem == BOSCH_B)
        printf("Brake System : Bosch\n");

    if (config.steeringSystem == BOSCH_S)
        printf("SteeringSystem : Bosch\n");
    else if (config.steeringSystem == MOBIS)
        printf("SteeringSystem : Mobis\n");

    printf("자동차가 동작됩니다.\n");
}

void testProducedCar(const CarConfig& config)
{
    if (isValidCheck(config))
    {
        printf("자동차 부품 조합 테스트 결과 : PASS\n");
        return;
    }

    printf("자동차 부품 조합 테스트 결과 : FAIL\n");

    if (config.carType == SEDAN && config.brakeSystem == CONTINENTAL)
        printf("Sedan에는 Continental제동장치 사용 불가\n");
    else if (config.carType == SUV && config.engine == TOYOTA)
        printf("SUV에는 TOYOTA엔진 사용 불가\n");
    else if (config.carType == TRUCK && config.engine == WIA)
        printf("Truck에는 WIA엔진 사용 불가\n");
    else if (config.carType == TRUCK && config.brakeSystem == MANDO)
        printf("Truck에는 Mando제동장치 사용 불가\n");
    else if (config.brakeSystem == BOSCH_B && config.steeringSystem != BOSCH_S)
        printf("Bosch제동장치에는 Bosch조향장치 이외 사용 불가\n");
}

#endif
