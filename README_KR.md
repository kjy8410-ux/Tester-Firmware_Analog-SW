# Tester Firmware – Analog Switch (MP4835AGRD)

## 개요 (Overview)

본 프로젝트는 Arduino Mega 기반의 아날로그 스위치 자동 테스트 시스템을 위한 펌웨어이다.
MUX 스캐닝과 ADC 기반 검증 방식을 사용하여 다수의 아날로그 스위치 채널을 검사하도록 설계되었다.

해당 시스템은 여러 개의 Sub Board를 제어하며, 모니터링 채널의 아날로그 전압 값을 분석하여 불량을 판별한다.

---

## 주요 기능 (Key Features)

* 6000CH 이상의 아날로그 스위치 채널 테스트 지원
* MUX 기반 채널 스캐닝 (Address + Bank 선택 구조)
* ADC 값 기반 Pass / Fail 판정
* Left / Right Bank 독립 검사
* 버튼 기반 제어 (DIN ON / TEST / ESTOP)
* LED 상태 표시 (Active-Low 방식)
* 시리얼 로그를 통한 불량 채널 출력

---

## 시스템 구조 (System Architecture)

### 테스트 동작 흐름 (Test Flow)

1. DIN (Digital Input) 활성화
2. 신호 안정화 대기
3. MUX를 통해 전체 채널 순차 스캔
4. ADC 값 측정
5. 기준값(Threshold)과 비교
6. FAIL 채널 시리얼 출력
7. IDLE 상태로 복귀

---

## 하드웨어 구성 (Hardware Configuration)

### MCU

* Arduino Mega 2560

### 인터페이스

* MUX Address 라인: S0 ~ S3
* Bank 선택 (Left / Right): 각 3-bit 구성
* ADC 입력: A0 (모니터링 채널)

### 제어 신호

* DIN Buffer 4채널
* 버튼 입력:

  * DIN ON
  * DIN OFF
  * TEST
  * ESTOP

### 상태 표시 LED (Active Low)

* IDLE
* DIN ON
* TEST RUNNING
* LEFT FAIL
* RIGHT FAIL
* ESTOP

---

## 핀 맵 (Pin Mapping)

| 기능                | 핀       |
| ----------------- | ------- |
| MUX Address       | 22 ~ 25 |
| Left Bank Select  | 26 ~ 28 |
| Right Bank Select | 29 ~ 31 |
| DIN 제어            | 32 ~ 35 |
| 버튼 입력             | 38 ~ 41 |
| LED 출력            | 42 ~ 47 |
| ADC               | A0      |

---

## 테스트 로직 (Test Logic)

### 채널 계산 방식

* Channel = Bank * 16 + Address

### ADC 판정 기준

* Left Threshold: adcOkMaxLeft = 150
* Right Threshold: adcOkMaxRight = 250

ADC 값이 기준값을 초과할 경우 해당 채널을 FAIL로 판정한다.

---

## 시리얼 출력 예시 (Serial Output Example)

```id="1t8v6z"
TEST START
[FAIL][LEFT] CH=10 (BANK=0 ADDR=10) ADC=151
[FAIL][RIGHT] CH=41 (BANK=2 ADDR=9) ADC=260
TEST END
```

---

## 상태 머신 (State Machine)

* INIT
* IDLE
* DIN_ON
* TEST_RUNNING
* ESTOP

### 상태별 동작

* IDLE: 사용자 입력 대기
* DIN_ON: DIN 강제 활성화 상태
* TEST_RUNNING: 전체 채널 검사 수행
* ESTOP: 즉시 동작 중지 (5초 유지 후 복귀)

---

## 타이밍 파라미터 (Timing Parameters)

| 항목         | 값         |
| ---------- | --------- |
| Test Delay | 12,000 ms |
| MUX 안정화 시간 | 10,000 µs |
| ADC 샘플링    | 8회 평균     |

---

## 구현 특징 (Implementation Highlights)

* ADC 평균값을 활용한 노이즈 감소
* 안정적인 측정을 위한 MUX 전환 후 딜레이 적용
* Left / Right Bank 분리 제어 (상호 간섭 방지)
* Active-Low LED 제어 방식
* MUX, DIN, ADC 기능별 저수준 함수 분리 구조

---

## 향후 개선 사항 (Future Improvements)

* C++ 기반 모듈 구조로 리팩토링 (.cpp / .h 분리)
* UART 명령 기반 제어 인터페이스 추가 (PC 연동)
* 로그 저장 기능 (SD 카드 또는 외부 인터페이스)
* 동적 Threshold 보정 기능
* Python 기반 GUI 모니터링 툴 개발

---

## 개발 환경 (Development Environment)

* Arduino IDE
* Target Board: Arduino Mega 2560
* Baud Rate: 115200

---

## 작성자 참고 사항 (Author Notes)

본 프로젝트는 다수의 아날로그 스위치 IC를 검사하기 위한 대규모 하드웨어 검증 시스템의 일부로 개발되었다.

다음과 같은 임베디드 시스템 설계 요소를 포함한다:

* 하드웨어 제어
* 신호 안정성 고려
* 자동화 테스트 로직
* 불량 검출 시스템

---
