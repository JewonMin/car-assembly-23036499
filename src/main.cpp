#include "../include/CarAssembler.h"
#include "../include/shared/CarValidator.h"
#include "../include/shared/utils.h"
#include <cstdio>
#include <memory>

void runCar(const Car& car, const CarValidator& validator) {
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

void testCar(const Car& car, const CarValidator& validator) {
    if (validator.isValid(car)) {
        printf("자동차 부품 조합 테스트 결과 : PASS\n");
        return;
    }
    printf("자동차 부품 조합 테스트 결과 : FAIL\n");
    printf("%s\n", validator.getFailReason(car).c_str());
}

int main() {
    CarAssembler assembler(
        { std::make_shared<SedanType>(),  std::make_shared<SuvType>(),  std::make_shared<TruckType>() },
        { std::make_shared<GmEngine>(),   std::make_shared<ToyotaEngine>(),
          std::make_shared<WiaEngine>(),  std::make_shared<BrokenEngine>() },
        { std::make_shared<MandoBrake>(), std::make_shared<ContinentalBrake>(), std::make_shared<BoschBrake>() },
        { std::make_shared<BoschSteering>(), std::make_shared<MobisSteering>() }
    );
    CarValidator validator = makeDefaultValidator();
    Car car;

    while (true) {
        if (!assembler.assemble(car)) { printf("바이바이\n"); break; }

        while (true) {
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
