# Tester-Firmware_Analog-SW

Arduino Mega 기반 아날로그 스위치 제어 및 검사 펌웨어입니다.

본 코드는 Sub Board에 연결된 다수의 아날로그 스위치를 시리얼 방식으로 제어하기 위해 작성되었습니다.  
각 스위치 채널은 `1 = ON`, `0 = OFF` 형태로 코드에 정의되며, Arduino에서 DIN / CLK / LEB 신호를 이용해 데이지체인 구조로 데이터를 전송합니다.

## 주요 기능

- Arduino Mega 기반 아날로그 스위치 제어
- 4개 구역별 LEB 제어
  - LU
  - LD
  - RU
  - RD
- DIN / CLK 기반 시리얼 데이터 전송
- 채널별 ON / OFF 상태를 `1`, `0`으로 직접 설정
- 버튼 입력을 통한 DIN ON, DIN OFF, TEST, ESTOP 제어
- MUX 및 ADC를 이용한 좌측 / 우측 채널 검사
- 시리얼 모니터를 통한 상태 확인 및 명령 입력

## 핀 구성

### LED

| 기능 | Arduino Pin |
|---|---:|
| IDLE LED | 42 |
| DIN ON LED | 43 |
| TEST LED | 44 |
| LEFT FAIL LED | 45 |
| RIGHT FAIL LED | 46 |
| ESTOP LED | 47 |

### Button

| 기능 | Arduino Pin |
|---|---:|
| DIN ON | 38 |
| DIN OFF | 39 |
| TEST | 40 |
| ESTOP | 41 |

### MUX

| 기능 | Arduino Pin |
|---|---:|
| MUX S0 | 22 |
| MUX S1 | 23 |
| MUX S2 | 24 |
| MUX S3 | 25 |
| LEFT BANK SEL 0 | 26 |
| LEFT BANK SEL 1 | 27 |
| LEFT BANK SEL 2 | 28 |
| RIGHT BANK SEL 0 | 29 |
| RIGHT BANK SEL 1 | 30 |
| RIGHT BANK SEL 2 | 31 |

### Serial DIN Control

| 기능 | Arduino Pin |
|---|---:|
| LEB_LU | 32 |
| LEB_LD | 33 |
| LEB_RU | 34 |
| LEB_RD | 35 |
| DIN DATA | 36 |
| DIN CLK | 37 |

## 채널 제어 방식

코드 내부에서 각 채널은 다음과 같이 `0` 또는 `1`로 정의됩니다.

```cpp
0 = Analog Switch OFF
1 = Analog Switch ON
