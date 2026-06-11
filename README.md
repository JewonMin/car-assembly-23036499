# car_assemble

절차지향 Legacy C++ 코드를 OOP + OCP 구조로 리팩토링한 차량 조립 시뮬레이터입니다.

---

## 리팩토링 개요

원본 코드(`assemblyCar`)는 전역 배열, 매직 넘버, 중복 검증 로직을 가진 절차지향 구조였습니다.  
PDF 가이드(Day2_1_Agentic Engineering, p.20-24)에 따라 5단계 리팩토링을 진행했습니다.

| Phase | 범위 | 주요 변경 |
|-------|------|-----------|
| **Phase 1** | Method Level | 네이밍 통일, `bool` 반환 타입, dead code 제거, busy-wait → `sleep_for`, `std::string` 입력 처리 |
| **Phase 2** | Data Structure | 전역 `stack[]` → `CarConfig` 구조체, 검증 단일 진입점 확보 |
| **Phase 3** | Class Level | `Car` / `CarValidator` / `CarAssembler` 클래스 분리, UI·비즈니스 로직 분리 |
| **Phase 4** | 확장성 (OCP) | 인터페이스 계층 도입, 규칙 객체 패턴, 의존성 주입, 유닛 테스트 14개 |
| **Phase 5** | 파일 분리 | 제조 순서 기반 폴더 구조, `#ifdef UNIT_TESTING` 제거, `test/` 독립 분리 |

---

## 프로젝트 구조

**제조 순서(1단계 차종 선택 → 2단계 부품 조립)가 폴더 구조에 직접 반영**되어 있습니다.

```
car_assemble/
│
├── include/
│   ├── step1_type_selection/       ← [1단계] 자동차 타입 선택
│   │   ├── ICarType.h              ICarType 인터페이스 + Sedan / SUV / Truck
│   │   └── CarTypeSelector.h       차종 선택 UI 담당
│   │
│   ├── step2_part_assembly/        ← [2단계] 부품 조립 (엔진 / 제동 / 조향)
│   │   ├── IEngine.h               IEngine + GM / Toyota / WIA / Broken
│   │   ├── IBrakeSystem.h          IBrakeSystem + Mando / Continental / Bosch
│   │   ├── ISteeringSystem.h       ISteeringSystem + Bosch / Mobis
│   │   └── CarPartAssembler.h      부품 3종 선택 UI 담당
│   │
│   ├── shared/                     ← 두 단계 공통 타입 · 검증
│   │   ├── Car.h                   Car 구조체 (shared_ptr 기반)
│   │   ├── IValidationRule.h       IValidationRule + 규칙 구현체 3개
│   │   ├── CarValidator.h          CarValidator (규칙 목록 기반)
│   │   └── utils.h                 CLEAR_SCREEN / EXIT_SIGNAL / delayMs / readInput
│   │
│   └── CarAssembler.h              ← Orchestrator: step1 → step2 순서 조합
│
├── src/                            ← include 와 동일한 폴더 구조
│   ├── step1_type_selection/CarTypeSelector.cpp
│   ├── step2_part_assembly/CarPartAssembler.cpp
│   ├── shared/CarValidator.cpp
│   ├── CarAssembler.cpp
│   └── main.cpp
│
└── test/
    └── test_main.cpp               ← Google Mock 유닛 테스트 14개
```

---

## 핵심 설계 원칙

### 제조 순서의 가시성

`CarAssembler::assemble()`을 읽으면 제조 순서가 코드에서 바로 보입니다.

```cpp
bool CarAssembler::assemble(Car& car) {
    while (true) {
        if (!typeSelector_.select(car))              return false;  // 1단계: 차종 선택
        auto result = partAssembler_.assemble(car);
        if (result == PartAssembleResult::Done)      return true;   // 조립 완료
        if (result == PartAssembleResult::Exit)      return false;  // 종료
        // BackToTypeSelect → 루프 반복
    }
}
```

### Open-Closed Principle

새 차종·부품 공급사를 추가할 때 기존 클래스를 수정하지 않아도 됩니다.

```cpp
// 새 차종 추가: ICarType 구현체만 작성
class VanType : public ICarType {
public: std::string name() const override { return "Van"; }
};

// 새 규칙 추가: IValidationRule 구현체만 작성
class NewRule : public IValidationRule { ... };
validator.addRule(std::make_shared<NewRule>());
```

### 의존성 주입

`CarAssembler`가 부품 목록을 외부에서 받아 메뉴를 자동 구성합니다.

```cpp
CarAssembler assembler(
    { make_shared<SedanType>(), make_shared<SuvType>(), make_shared<TruckType>() },
    { make_shared<GmEngine>(),  make_shared<ToyotaEngine>(), ... },
    ...
);
```

---

## 유닛 테스트

테스트 결과 상세 보기: [`test_results.html`](test_results.html)

```
[==========] 14 tests from 4 test suites ran. (0 ms total)
[  PASSED  ] 14 tests.
```

| Test Suite | 검증 대상 | 수 |
|---|---|---|
| `CarValidatorTest` | 통합 시나리오 + OCP 확장성 | 8 |
| `EngineCarTypeRuleTest` | 엔진-차종 호환성 규칙 | 2 |
| `BrakeCarTypeRuleTest` | 제동장치-차종 호환성 규칙 | 2 |
| `BoschBrakeSteeringRuleTest` | Bosch 제동-조향 연동 규칙 | 2 |

### 빌드 및 실행

```bat
rem 테스트 빌드 (Visual Studio 개발자 명령 프롬프트에서)
cl /EHsc /std:c++17 /MD /I"include" /I"<gmock-include>" ^
   src\shared\CarValidator.cpp test\test_main.cpp ^
   /Fe:test_runner.exe ^
   /link <gmock.lib> <gmock_main.lib>

test_runner.exe

rem 프로덕션 빌드
cl /EHsc /std:c++17 /MD /I"include" ^
   src\step1_type_selection\CarTypeSelector.cpp ^
   src\step2_part_assembly\CarPartAssembler.cpp ^
   src\shared\CarValidator.cpp ^
   src\CarAssembler.cpp src\main.cpp ^
   /Fe:car_assemble.exe
```

---

## 제한 조건 (PDF p.23)

| 조건 | 구현 위치 |
|------|-----------|
| Toyota 엔진 → SUV 사용 불가 | `step2_part_assembly/IEngine.h` — `ToyotaEngine::isCompatibleWith` |
| WIA 엔진 → Truck 사용 불가 | `step2_part_assembly/IEngine.h` — `WiaEngine::isCompatibleWith` |
| Continental 제동 → Sedan 사용 불가 | `step2_part_assembly/IBrakeSystem.h` — `ContinentalBrake::isCompatibleWith` |
| Mando 제동 → Truck 사용 불가 | `step2_part_assembly/IBrakeSystem.h` — `MandoBrake::isCompatibleWith` |
| Bosch 제동 → Bosch 조향 필수 | `shared/IValidationRule.h` — `BoschBrakeSteeringRule::isSatisfied` |
