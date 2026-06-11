# 리팩토링 계획 (assemble.cpp)

## 배경

PDF(Day2_1_Agentic Engineering) 20~24p 기반.  
절차지향 Legacy 코드를 OOP 구조로 전환하며 안전성·확장성·테스트 가능성을 확보한다.

### 현재 문제점 (PDF p.24)
- 절차지향식 코드 — 유지보수 어려움
- 안전하지 않은 문법 사용
- 확장성 미고려
- 유닛테스트 없음

---

## Phase 1. Method Level 리팩토링

> 함수 내부의 구현 방식, 반환 타입, 문법 수정

### 1-1. 네이밍 일관성 수정
- `selectbrakeSystem` → `selectBrakeSystem` (소문자 b → 대문자 B)
- `brakeSystem` enum → `BrakeSystem` (PascalCase 통일)

### 1-2. `isValidCheck()` 반환 타입 수정
```cpp
// Before
int isValidCheck()

// After
bool isValidCheck()
```
- int를 bool 대용으로 사용하는 unsafe 패턴 제거

### 1-3. `isValidCheck()` 중복 `return true` 제거
- 함수 마지막 `return true;` (323번 줄) — 절대 도달 불가 dead code 제거

### 1-4. `selectXxx()` 함수의 if 체인 → if-else 체인으로 교체
```cpp
// Before: 조건이 독립적이지 않아 매번 모든 if 평가
if (answer == 1) ...
if (answer == 2) ...

// After: 상호 배타적임을 명시
if (answer == 1) ...
else if (answer == 2) ...
else if (answer == 3) ...
```

### 1-5. `testProducedCar()` 내 중복 검증 로직 제거
- `testProducedCar()`가 `isValidCheck()`와 동일한 조건을 중복 구현 중
- `isValidCheck()` 결과를 재사용하도록 통합

### 1-6. `#define CLEAR_SCREEN` → `const std::string` 또는 `constexpr`
```cpp
// Before
#define CLEAR_SCREEN "\033[H\033[2J"

// After
constexpr const char* CLEAR_SCREEN = "\033[H\033[2J";
```

### 1-7. `delay()` busy-wait 제거 → `std::this_thread::sleep_for` 교체
```cpp
// Before: volatile int sum busy-wait (CPU 점유, 불정확)
void delay(int ms) { volatile int sum = 0; for(...) ... }

// After
#include <thread>
#include <chrono>
void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
```

### 1-8. 입력 처리: raw char 배열 + `strtok_s` → `std::string` + `std::getline`
```cpp
// Before (Windows 전용, unsafe)
char buf[100];
fgets(buf, sizeof(buf), stdin);
char *context = nullptr;
strtok_s(buf, "\r", &context);

// After (portable, safe)
std::string input;
std::getline(std::cin, input);
```

### 1-9. 매직 넘버 `4` (고장난 엔진) 제거
- `Engine` enum에 `BROKEN = 4` 항목 추가하거나 별도 플래그로 분리

---

## Phase 2. Data Structure Level 리팩토링

> 전역 상태 제거, 데이터 구조 명확화

### 2-1. 전역 `int stack[10]` 제거 → 구조체로 교체
```cpp
// Before
int stack[10];  // 전역, 인덱스 의미 불명확

// After
struct CarConfig {
    CarType    carType;
    Engine     engine;
    BrakeSystem brakeSystem;
    SteeringSystem steeringSystem;
};
```

### 2-2. `QuestionType` enum 역할 분리
- 현재 `QuestionType`이 UI 진행 step과 `stack[]` 인덱스 두 가지 역할을 겸함
- step 제어용 enum과 데이터 저장 구조를 분리

### 2-3. 검증 로직 단일 진입점 확보
- `runProducedCar()`와 `testProducedCar()` 모두 `isValidCheck(CarConfig)`를 통해 검증
- 검증 로직이 한 곳에만 존재하도록 보장

---

## Phase 3. Class Level 리팩토링

> 관심사 분리, OOP 구조 전환, 테스트 가능성 확보

### 3-1. `Car` 클래스 생성
```cpp
class Car {
public:
    CarType        type;
    Engine         engine;
    BrakeSystem    brakeSystem;
    SteeringSystem steeringSystem;
};
```

### 3-2. `CarValidator` 클래스 생성
- 검증 규칙을 독립 클래스로 분리 (유닛 테스트 대상)
- 제한조건 (PDF p.23) 5가지를 메서드로 표현
```cpp
class CarValidator {
public:
    bool isValid(const Car& car) const;
private:
    bool checkBoschCompatibility(const Car& car) const;
    bool checkTypePartCompatibility(const Car& car) const;
};
```

### 3-3. `CarAssembler` 클래스 생성
- 조립 workflow(타입 선택 → 부품 선택 → 검증) 캡슐화
```cpp
class CarAssembler {
public:
    Car assemble();
private:
    CarType        selectCarType();
    Engine         selectEngine();
    BrakeSystem    selectBrakeSystem();
    SteeringSystem selectSteeringSystem();
};
```

### 3-4. UI 레이어와 비즈니스 로직 분리
- `main()`은 UI 루프만 담당
- 입력 파싱, 검증, 출력을 각 클래스 책임으로 위임

### 3-5. 유닛 테스트 작성 (Google Mock)
- `#ifdef _DEBUG` 블록 활용 (이미 gmock 연결됨)
- `CarValidator` 검증 규칙 5가지 각각 테스트
- 유효 조합 / 무효 조합 케이스 망라

```cpp
TEST(CarValidatorTest, BoschBrake_RequiresBoschSteering) { ... }
TEST(CarValidatorTest, Sedan_CannotUse_Continental)      { ... }
TEST(CarValidatorTest, SUV_CannotUse_Toyota)             { ... }
TEST(CarValidatorTest, Truck_CannotUse_WIA)              { ... }
TEST(CarValidatorTest, Truck_CannotUse_Mando)            { ... }
```

---

## Phase 4. 확장성 (OCP 적용)

> 새 차종·부품 공급사 추가 시 기존 코드를 수정하지 않아도 되는 구조로 전환.  
> Open-Closed Principle: 확장에는 열려 있고, 수정에는 닫혀 있어야 한다.

### 현재 구조의 문제
- 새 차종(예: Van) 추가 → `CarType` enum 수정 + `CarValidator` 조건문 수정 + `CarAssembler` 메뉴 수정 (3곳 동시 수정)
- 새 부품 공급사 추가 → 동일하게 여러 곳 수정 필요
- 제한조건이 `CarValidator` 내부에 if문으로 하드코딩되어 규칙 추가 시 클래스 수정 불가피

---

### 4-1. 부품 인터페이스 도입

엔진·제동장치·조향장치를 추상 인터페이스로 분리.  
새 공급사 추가 시 구현체 클래스만 추가하면 되고, 기존 코드는 수정하지 않는다.

```cpp
class IEngine {
public:
    virtual std::string name() const = 0;
    virtual ~IEngine() = default;
};

class GmEngine : public IEngine {
public:
    std::string name() const override { return "GM"; }
};

class ToyotaEngine : public IEngine {
public:
    std::string name() const override { return "TOYOTA"; }
};

// 새 공급사 추가 시: 기존 코드 수정 없이 클래스만 추가
class FordEngine : public IEngine {
public:
    std::string name() const override { return "FORD"; }
};
```

동일 구조를 `IBrakeSystem`, `ISteeringSystem`에도 적용.

---

### 4-2. 차종 인터페이스 도입

차종도 인터페이스로 추상화.  
새 차종 추가 시 구현체 클래스만 추가하면 되고, `CarValidator`·`CarAssembler` 수정 불필요.

```cpp
class ICarType {
public:
    virtual std::string name() const = 0;
    virtual ~ICarType() = default;
};

class Sedan : public ICarType {
public:
    std::string name() const override { return "Sedan"; }
};

// 새 차종 추가: 기존 코드 수정 없음
class Van : public ICarType {
public:
    std::string name() const override { return "Van"; }
};
```

---

### 4-3. `Car` 클래스를 인터페이스 기반으로 변경

enum 기반 `Car`에서 인터페이스 포인터 기반으로 교체.

```cpp
// Before
class Car {
public:
    CarType        type;
    Engine         engine;
    BrakeSystem    brakeSystem;
    SteeringSystem steeringSystem;
};

// After
class Car {
public:
    std::shared_ptr<ICarType>       type;
    std::shared_ptr<IEngine>        engine;
    std::shared_ptr<IBrakeSystem>   brakeSystem;
    std::shared_ptr<ISteeringSystem> steeringSystem;
};
```

---

### 4-4. `CarValidator` 규칙을 데이터로 분리

제한조건을 if문 하드코딩 대신 규칙 객체 목록으로 관리.  
새 제한조건 추가 시 `CarValidator` 클래스 수정 없이 규칙 목록에 추가만 하면 된다.

```cpp
class IValidationRule {
public:
    virtual bool isSatisfied(const Car& car) const = 0;
    virtual std::string failReason() const = 0;
    virtual ~IValidationRule() = default;
};

class CarValidator {
public:
    void addRule(std::shared_ptr<IValidationRule> rule);
    bool isValid(const Car& car) const;
    std::string getFailReason(const Car& car) const;
private:
    std::vector<std::shared_ptr<IValidationRule>> rules;
};
```

제한조건 구현 예시:

```cpp
class BoschCompatibilityRule : public IValidationRule {
public:
    bool isSatisfied(const Car& car) const override {
        // Bosch 제동장치 사용 시 Bosch 조향장치 필수
        ...
    }
    std::string failReason() const override {
        return "Bosch제동장치에는 Bosch조향장치 이외 사용 불가";
    }
};
```

---

### 4-5. `CarAssembler`에 부품 목록 주입 (의존성 주입)

`CarAssembler`가 내부에서 부품 목록을 하드코딩하는 대신,  
외부에서 주입받아 새 부품·차종이 자동으로 메뉴에 반영되도록 한다.

```cpp
class CarAssembler {
public:
    CarAssembler(
        std::vector<std::shared_ptr<ICarType>>        carTypes,
        std::vector<std::shared_ptr<IEngine>>         engines,
        std::vector<std::shared_ptr<IBrakeSystem>>    brakeSystems,
        std::vector<std::shared_ptr<ISteeringSystem>> steeringSystems
    );
    bool assemble(Car& car);
};
```

`main()`에서 부품 목록을 조립해 주입:

```cpp
int main() {
    CarAssembler assembler(
        { std::make_shared<Sedan>(), std::make_shared<Suv>(), std::make_shared<Truck>() },
        { std::make_shared<GmEngine>(), std::make_shared<ToyotaEngine>(), std::make_shared<WiaEngine>() },
        ...
    );
}
```

---

### 4-6. 유닛 테스트 보강

- `IValidationRule` 구현체별 단위 테스트
- `CarValidator`에 규칙 동적 추가/제거 테스트
- 새 차종·부품이 조합 검증에 올바르게 반영되는지 테스트

---

## 진행 순서 요약

| 순서 | Phase | 항목 |
|------|-------|------|
| 1 | Method | 네이밍 일관성 (1-1) |
| 2 | Method | `isValidCheck` 반환 타입 bool 변경 (1-2) |
| 3 | Method | dead code 제거 (1-3) |
| 4 | Method | if 체인 → if-else (1-4) |
| 5 | Method | `testProducedCar` 중복 제거 (1-5) |
| 6 | Method | `#define` 제거 (1-6) |
| 7 | Method | `delay()` busy-wait 제거 (1-7) |
| 8 | Method | 입력 처리 안전화 (1-8) |
| 9 | Method | 매직 넘버 제거 (1-9) |
| 10 | Data Structure | 전역 `stack[]` → `CarConfig` 구조체 (2-1) |
| 11 | Data Structure | `QuestionType` 역할 분리 (2-2) |
| 12 | Data Structure | 검증 단일 진입점 (2-3) |
| 13 | Class | `Car` 클래스 (3-1) |
| 14 | Class | `CarValidator` 클래스 (3-2) |
| 15 | Class | `CarAssembler` 클래스 (3-3) |
| 16 | Class | UI / 비즈니스 로직 분리 (3-4) |
| 17 | Class | 유닛 테스트 작성 (3-5) |
| 18 | 확장성 | 부품 인터페이스 도입 — `IEngine` / `IBrakeSystem` / `ISteeringSystem` (4-1) |
| 19 | 확장성 | 차종 인터페이스 도입 — `ICarType` (4-2) |
| 20 | 확장성 | `Car` 클래스를 인터페이스 포인터 기반으로 변경 (4-3) |
| 21 | 확장성 | `CarValidator` 규칙을 `IValidationRule` 목록으로 분리 (4-4) |
| 22 | 확장성 | `CarAssembler`에 부품 목록 의존성 주입 (4-5) |
| 23 | 확장성 | 유닛 테스트 보강 (4-6) |
