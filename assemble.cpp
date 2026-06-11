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

// 1-6: #define → constexpr
constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";

int stack[10];

void selectCarType(int answer);
void selectEngine(int answer);
void selectBrakeSystem(int answer);   // 1-1: selectbrakeSystem → selectBrakeSystem
void selectSteeringSystem(int answer);
void runProducedCar();
void testProducedCar();
void delay(int ms);

enum QuestionType
{
    CarType_Q,
    Engine_Q,
    BrakeSystem_Q,      // 1-1: brakeSystem_Q → BrakeSystem_Q
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
    BROKEN  // 1-9: 매직 넘버 4 → 명명된 열거값
};

enum BrakeSystem    // 1-1: brakeSystem → BrakeSystem
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

// 1-7: busy-wait → std::this_thread::sleep_for
void delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main()
{
    std::string input;  // 1-8: char buf[100] → std::string
    int step = CarType_Q;

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

        // 1-8: fgets + strtok_s → std::getline
        std::getline(std::cin, input);

        if (input == "exit")
        {
            printf("바이바이\n");
            break;
        }

        // 1-8: strtol + checkNumber → std::stoi + try-catch
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
            selectCarType(answer);
            delay(800);
            step = Engine_Q;
        }
        else if (step == Engine_Q)
        {
            selectEngine(answer);
            delay(800);
            step = BrakeSystem_Q;
        }
        else if (step == BrakeSystem_Q)
        {
            selectBrakeSystem(answer);
            delay(800);
            step = SteeringSystem_Q;
        }
        else if (step == SteeringSystem_Q)
        {
            selectSteeringSystem(answer);
            delay(800);
            step = Run_Test;
        }
        else if (step == Run_Test && answer == 1)
        {
            runProducedCar();
            delay(2000);
        }
        else if (step == Run_Test && answer == 2)
        {
            printf("Test...\n");
            delay(1500);
            testProducedCar();
            delay(2000);
        }
    }
}

void selectCarType(int answer)
{
    stack[CarType_Q] = answer;
    // 1-4: if 체인 → if-else 체인
    if (answer == SEDAN)
        printf("차량 타입으로 Sedan을 선택하셨습니다.\n");
    else if (answer == SUV)
        printf("차량 타입으로 SUV을 선택하셨습니다.\n");
    else if (answer == TRUCK)
        printf("차량 타입으로 Truck을 선택하셨습니다.\n");
}

void selectEngine(int answer)
{
    stack[Engine_Q] = answer;
    // 1-4: if 체인 → if-else 체인
    if (answer == GM)
        printf("GM 엔진을 선택하셨습니다.\n");
    else if (answer == TOYOTA)
        printf("TOYOTA 엔진을 선택하셨습니다.\n");
    else if (answer == WIA)
        printf("WIA 엔진을 선택하셨습니다.\n");
    else if (answer == BROKEN)
        printf("고장난 엔진을 선택하셨습니다.\n");
}

// 1-1: selectbrakeSystem → selectBrakeSystem
void selectBrakeSystem(int answer)
{
    stack[BrakeSystem_Q] = answer;
    // 1-4: if 체인 → if-else 체인
    if (answer == MANDO)
        printf("MANDO 제동장치를 선택하셨습니다.\n");
    else if (answer == CONTINENTAL)
        printf("CONTINENTAL 제동장치를 선택하셨습니다.\n");
    else if (answer == BOSCH_B)
        printf("BOSCH 제동장치를 선택하셨습니다.\n");
}

void selectSteeringSystem(int answer)
{
    stack[SteeringSystem_Q] = answer;
    // 1-4: if 체인 → if-else 체인
    if (answer == BOSCH_S)
        printf("BOSCH 조향장치를 선택하셨습니다.\n");
    else if (answer == MOBIS)
        printf("MOBIS 조향장치를 선택하셨습니다.\n");
}

// 1-2: 반환 타입 int → bool / 1-3: 마지막 dead code return true 제거
bool isValidCheck()
{
    if (stack[CarType_Q] == SEDAN && stack[BrakeSystem_Q] == CONTINENTAL)
        return false;
    else if (stack[CarType_Q] == SUV && stack[Engine_Q] == TOYOTA)
        return false;
    else if (stack[CarType_Q] == TRUCK && stack[Engine_Q] == WIA)
        return false;
    else if (stack[CarType_Q] == TRUCK && stack[BrakeSystem_Q] == MANDO)
        return false;
    else if (stack[BrakeSystem_Q] == BOSCH_B && stack[SteeringSystem_Q] != BOSCH_S)
        return false;
    return true;
}

void runProducedCar()
{
    if (!isValidCheck())
    {
        printf("자동차가 동작되지 않습니다\n");
        return;
    }

    // 1-9: stack[Engine_Q] == 4 → BROKEN
    if (stack[Engine_Q] == BROKEN)
    {
        printf("엔진이 고장나있습니다.\n");
        printf("자동차가 움직이지 않습니다.\n");
        return;
    }

    // 1-4: if 체인 → if-else 체인
    if (stack[CarType_Q] == SEDAN)
        printf("Car Type : Sedan\n");
    else if (stack[CarType_Q] == SUV)
        printf("Car Type : SUV\n");
    else if (stack[CarType_Q] == TRUCK)
        printf("Car Type : Truck\n");

    if (stack[Engine_Q] == GM)
        printf("Engine : GM\n");
    else if (stack[Engine_Q] == TOYOTA)
        printf("Engine : TOYOTA\n");
    else if (stack[Engine_Q] == WIA)
        printf("Engine : WIA\n");

    if (stack[BrakeSystem_Q] == MANDO)
        printf("Brake System : Mando\n");
    else if (stack[BrakeSystem_Q] == CONTINENTAL)
        printf("Brake System : Continental\n");
    else if (stack[BrakeSystem_Q] == BOSCH_B)
        printf("Brake System : Bosch\n");

    if (stack[SteeringSystem_Q] == BOSCH_S)
        printf("SteeringSystem : Bosch\n");
    else if (stack[SteeringSystem_Q] == MOBIS)
        printf("SteeringSystem : Mobis\n");

    printf("자동차가 동작됩니다.\n");
}

// 1-5: isValidCheck() 재사용으로 중복 검증 로직 제거
void testProducedCar()
{
    if (isValidCheck())
    {
        printf("자동차 부품 조합 테스트 결과 : PASS\n");
        return;
    }

    printf("자동차 부품 조합 테스트 결과 : FAIL\n");

    if (stack[CarType_Q] == SEDAN && stack[BrakeSystem_Q] == CONTINENTAL)
        printf("Sedan에는 Continental제동장치 사용 불가\n");
    else if (stack[CarType_Q] == SUV && stack[Engine_Q] == TOYOTA)
        printf("SUV에는 TOYOTA엔진 사용 불가\n");
    else if (stack[CarType_Q] == TRUCK && stack[Engine_Q] == WIA)
        printf("Truck에는 WIA엔진 사용 불가\n");
    else if (stack[CarType_Q] == TRUCK && stack[BrakeSystem_Q] == MANDO)
        printf("Truck에는 Mando제동장치 사용 불가\n");
    else if (stack[BrakeSystem_Q] == BOSCH_B && stack[SteeringSystem_Q] != BOSCH_S)
        printf("Bosch제동장치에는 Bosch조향장치 이외 사용 불가\n");
}

#endif
