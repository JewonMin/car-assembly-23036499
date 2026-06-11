# car_assemble

절차지향 Legacy C++ 코드를 OOP + OCP 구조로 리팩토링한 차량 조립 시뮬레이터입니다.

---

## 리팩토링 개요

원본 코드(`assemblyCar`)는 전역 배열, 매직 넘버, 중복 검증 로직을 가진 절차지향 구조였습니다.  
PDF 가이드(Day2_1_Agentic Engineering, p.20-24)에 따라 4단계 리팩토링을 진행했습니다.

| Phase | 범위 | 주요 변경 |
|-------|------|-----------|
| **Phase 1** | Method Level | 네이밍 통일, `bool` 반환 타입, dead code 제거, busy-wait → `sleep_for`, `std::string` 입력 처리 |
| **Phase 2** | Data Structure | 전역 `stack[]` → `CarConfig` 구조체, 검증 단일 진입점 확보 |
| **Phase 3** | Class Level | `Car` / `CarValidator` / `CarAssembler` 클래스 분리, UI·비즈니스 로직 분리 |
| **Phase 4** | 확장성 (OCP) | 인터페이스 계층 도입, 규칙 객체 패턴, 의존성 주입, 유닛 테스트 14개 |

---

## 코드 구조

모든 코드는 `assemble.cpp` 단일 파일에 있으며, 빌드 매크로로 **테스트 빌드**와 **프로덕션 빌드**를 분리합니다.

```
assemble.cpp
│
├── [공유 영역] ─────────────────────────────────────────────────
│   │
│   ├── 차종 인터페이스
│   │   ├── ICarType          (interface)
│   │   ├── SedanType
│   │   ├── SuvType
│   │   └── TruckType
│   │
│   ├── 엔진 인터페이스
│   │   ├── IEngine           (interface: name, isBroken, isCompatibleWith)
│   │   ├── GmEngine
│   │   ├── ToyotaEngine      (SUV 불가)
│   │   ├── WiaEngine         (Truck 불가)
│   │   └── BrokenEngine
│   │
│   ├── 제동장치 인터페이스
│   │   ├── IBrakeSystem      (interface: name, isCompatibleWith, requiresBoschSteering)
│   │   ├── MandoBrake        (Truck 불가)
│   │   ├── ContinentalBrake  (Sedan 불가)
│   │   └── BoschBrake        (Bosch 조향장치 필수)
│   │
│   ├── 조향장치 인터페이스
│   │   ├── ISteeringSystem   (interface: name, isBoschType)
│   │   ├── BoschSteering
│   │   └── MobisSteering
│   │
│   ├── Car                   (shared_ptr 기반 부품 컨테이너)
│   │
│   ├── 검증 규칙 인터페이스
│   │   ├── IValidationRule   (interface: isSatisfied, failReason)
│   │   ├── EngineCarTypeRule
│   │   ├── BrakeCarTypeRule
│   │   └── BoschBrakeSteeringRule
│   │
│   └── CarValidator          (규칙 목록 기반: addRule / isValid / getFailReason)
│
├── [#ifdef UNIT_TESTING] ───────────────────────────────────────
│   └── Google Mock 유닛 테스트 14개
│       ├── CarValidatorTest       (8개 — 통합 시나리오 + OCP 확장성)
│       ├── EngineCarTypeRuleTest  (2개)
│       ├── BrakeCarTypeRuleTest   (2개)
│       └── BoschBrakeSteeringRuleTest (2개)
│
└── [#else — Production] ────────────────────────────────────────
    ├── CarAssembler  (부품 목록 의존성 주입, 단계별 조립 UI)
    ├── runCar()      (검증 후 차량 정보 출력)
    ├── testCar()     (PASS / FAIL + 실패 이유 출력)
    └── main()        (CarAssembler + CarValidator 조립 및 루프)
```

### 핵심 설계 원칙

**Open-Closed Principle** — 새 차종·부품 공급사를 추가할 때 기존 클래스를 수정하지 않아도 됩니다.

```cpp
// 새 차종 추가: ICarType 구현체만 작성
class VanType : public ICarType {
public: std::string name() const override { return "Van"; }
};

// 새 규칙 추가: IValidationRule 구현체만 작성, CarValidator는 수정 없음
class NewRule : public IValidationRule { ... };
validator.addRule(std::make_shared<NewRule>());
```

**의존성 주입** — `CarAssembler`가 부품 목록을 외부에서 받아 메뉴를 자동 구성합니다.

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
[==========] 14 tests from 4 test suites.
[  PASSED  ] 14 tests.
```

### 빌드 및 실행

```bat
rem 테스트 빌드 (Visual Studio 개발자 명령 프롬프트에서)
cl /EHsc /std:c++17 /MD /DUNIT_TESTING ^
   /I"<gmock-include>" assemble.cpp ^
   /Fe:test_runner.exe ^
   /link "<gmock.lib>" "<gmock_main.lib>"

test_runner.exe

rem 프로덕션 빌드
cl /EHsc /std:c++17 /MD assemble.cpp /Fe:car_assemble.exe
```

---

## 제한 조건 (PDF p.23)

| 조건 | 구현 위치 |
|------|-----------|
| Toyota 엔진 → SUV 사용 불가 | `ToyotaEngine::isCompatibleWith` |
| WIA 엔진 → Truck 사용 불가 | `WiaEngine::isCompatibleWith` |
| Continental 제동 → Sedan 사용 불가 | `ContinentalBrake::isCompatibleWith` |
| Mando 제동 → Truck 사용 불가 | `MandoBrake::isCompatibleWith` |
| Bosch 제동 → Bosch 조향 필수 | `BoschBrakeSteeringRule::isSatisfied` |
