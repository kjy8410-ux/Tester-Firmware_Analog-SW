#include <Arduino.h>

// =====================================================
// SERIAL
// =====================================================
#define SERIAL_DEBUG true
#define SERIAL_BAUD 115200

// =====================================================
// LED (ACTIVE LOW)
// =====================================================
#define LED_ON  LOW
#define LED_OFF HIGH

const uint8_t PIN_LED_IDLE   = 42;
const uint8_t PIN_LED_DINON  = 43;
const uint8_t PIN_LED_TEST   = 44;
const uint8_t PIN_LED_LEFT   = 45;
const uint8_t PIN_LED_RIGHT  = 46;
const uint8_t PIN_LED_ESTOP  = 47;

// =====================================================
// BUTTON
// =====================================================
const uint8_t PIN_BTN_DIN_ON  = 38;
const uint8_t PIN_BTN_DIN_OFF = 39;
const uint8_t PIN_BTN_TEST    = 40;
const uint8_t PIN_BTN_ESTOP   = 41;

// =====================================================
// ADC
// =====================================================
const uint8_t PIN_ADC = A0;

// =====================================================
// MUX
// =====================================================
const uint8_t PIN_MUX_S0 = 22;
const uint8_t PIN_MUX_S1 = 23;
const uint8_t PIN_MUX_S2 = 24;
const uint8_t PIN_MUX_S3 = 25;

const uint8_t PIN_MUX_SEL_L0 = 26;
const uint8_t PIN_MUX_SEL_L1 = 27;
const uint8_t PIN_MUX_SEL_L2 = 28;

const uint8_t PIN_MUX_SEL_R0 = 29;
const uint8_t PIN_MUX_SEL_R1 = 30;
const uint8_t PIN_MUX_SEL_R2 = 31;

// =====================================================
// DIN
// =====================================================
const uint8_t PIN_DIN_BUF01 = 32;
const uint8_t PIN_DIN_BUF02 = 33;
const uint8_t PIN_DIN_BUF03 = 34;
const uint8_t PIN_DIN_BUF04 = 35;

// =====================================================
// PARAM
// =====================================================
const uint32_t TEST_DELAY_MS = 12000;
const uint16_t MUX_SETTLE_US = 10000;

int adcOkMaxLeft  = 150;
int adcOkMaxRight = 250;

// =====================================================
// STATE
// =====================================================
enum State {
  INIT,
  IDLE,
  DIN_ON,
  TEST_RUNNING,
  ESTOP
};

State state = INIT;
unsigned long stateTime = 0;

bool leftFail  = false;
bool rightFail = false;

// =====================================================
// LOW LEVEL
// =====================================================
void setDin(bool on) {
  digitalWrite(PIN_DIN_BUF01, on ? LOW : HIGH);
  digitalWrite(PIN_DIN_BUF02, on ? LOW : HIGH);
  digitalWrite(PIN_DIN_BUF03, on ? LOW : HIGH);
  digitalWrite(PIN_DIN_BUF04, on ? LOW : HIGH);
}

void setMuxAddr(uint8_t a) {
  digitalWrite(PIN_MUX_S0, a & 1);
  digitalWrite(PIN_MUX_S1, a & 2);
  digitalWrite(PIN_MUX_S2, a & 4);
  digitalWrite(PIN_MUX_S3, a & 8);
}

void setLeftBank(uint8_t b) {
  digitalWrite(PIN_MUX_SEL_L0, b & 1);
  digitalWrite(PIN_MUX_SEL_L1, b & 2);
  digitalWrite(PIN_MUX_SEL_L2, b & 4);
}

void setRightBank(uint8_t b) {
  digitalWrite(PIN_MUX_SEL_R0, b & 1);
  digitalWrite(PIN_MUX_SEL_R1, b & 2);
  digitalWrite(PIN_MUX_SEL_R2, b & 4);
}

void parkLeft()  { setLeftBank(7); }
void parkRight() { setRightBank(7); }

// =====================================================
// ADC
// =====================================================
int readADC() {
  long sum = 0;
  for (int i=0;i<8;i++) {
    sum += analogRead(PIN_ADC);
    delayMicroseconds(200);
  }
  return sum / 8;
}

// =====================================================
// SCAN (채널 출력 포함)
// =====================================================
bool scanLeft() {
  parkRight();

  bool fail = false;

  for (int b=0;b<6;b++) {
    setLeftBank(b);

    for (int a=0;a<16;a++) {
      setMuxAddr(a);
      delayMicroseconds(MUX_SETTLE_US);

      int v = readADC();
      int ch = b * 16 + a;

      if (v > adcOkMaxLeft) {
        fail = true;

        if (SERIAL_DEBUG) {
          Serial.print("[FAIL][LEFT] CH=");
          Serial.print(ch);
          Serial.print(" (BANK=");
          Serial.print(b);
          Serial.print(" ADDR=");
          Serial.print(a);
          Serial.print(") ADC=");
          Serial.println(v);
        }
      }
    }
  }

  return fail;
}

bool scanRight() {
  parkLeft();

  bool fail = false;

  for (int b=0;b<7;b++) {
    setRightBank(b);

    for (int a=0;a<16;a++) {
      setMuxAddr(a);
      delayMicroseconds(MUX_SETTLE_US);

      int v = readADC();
      int ch = b * 16 + a;

      if (v > adcOkMaxRight) {
        fail = true;

        if (SERIAL_DEBUG) {
          Serial.print("[FAIL][RIGHT] CH=");
          Serial.print(ch);
          Serial.print(" (BANK=");
          Serial.print(b);
          Serial.print(" ADDR=");
          Serial.print(a);
          Serial.print(") ADC=");
          Serial.println(v);
        }
      }
    }
  }

  return fail;
}

// =====================================================
// LED
// =====================================================
void updateLED() {

  digitalWrite(PIN_LED_IDLE,  LED_OFF);
  digitalWrite(PIN_LED_DINON, LED_OFF);
  digitalWrite(PIN_LED_TEST,  LED_OFF);
  digitalWrite(PIN_LED_LEFT,  LED_OFF);
  digitalWrite(PIN_LED_RIGHT, LED_OFF);
  digitalWrite(PIN_LED_ESTOP, LED_OFF);

  switch (state) {

    case IDLE:
      digitalWrite(PIN_LED_IDLE, LED_ON);
      if (leftFail)  digitalWrite(PIN_LED_LEFT, LED_ON);
      if (rightFail) digitalWrite(PIN_LED_RIGHT, LED_ON);
      break;

    case DIN_ON:
      digitalWrite(PIN_LED_DINON, LED_ON);
      break;

    case TEST_RUNNING:
      digitalWrite(PIN_LED_TEST, LED_ON);
      break;

    case ESTOP:
      digitalWrite(PIN_LED_ESTOP, LED_ON);
      break;
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  if (SERIAL_DEBUG) {
    Serial.begin(SERIAL_BAUD);
    delay(500);
    Serial.println("SYSTEM START");
  }

  pinMode(PIN_LED_IDLE, OUTPUT);
  pinMode(PIN_LED_DINON, OUTPUT);
  pinMode(PIN_LED_TEST, OUTPUT);
  pinMode(PIN_LED_LEFT, OUTPUT);
  pinMode(PIN_LED_RIGHT, OUTPUT);
  pinMode(PIN_LED_ESTOP, OUTPUT);

  pinMode(PIN_BTN_DIN_ON, INPUT_PULLUP);
  pinMode(PIN_BTN_DIN_OFF, INPUT_PULLUP);
  pinMode(PIN_BTN_TEST, INPUT_PULLUP);
  pinMode(PIN_BTN_ESTOP, INPUT_PULLUP);

  pinMode(PIN_DIN_BUF01, OUTPUT);
  pinMode(PIN_DIN_BUF02, OUTPUT);
  pinMode(PIN_DIN_BUF03, OUTPUT);
  pinMode(PIN_DIN_BUF04, OUTPUT);

  pinMode(PIN_MUX_S0, OUTPUT);
  pinMode(PIN_MUX_S1, OUTPUT);
  pinMode(PIN_MUX_S2, OUTPUT);
  pinMode(PIN_MUX_S3, OUTPUT);

  pinMode(PIN_MUX_SEL_L0, OUTPUT);
  pinMode(PIN_MUX_SEL_L1, OUTPUT);
  pinMode(PIN_MUX_SEL_L2, OUTPUT);

  pinMode(PIN_MUX_SEL_R0, OUTPUT);
  pinMode(PIN_MUX_SEL_R1, OUTPUT);
  pinMode(PIN_MUX_SEL_R2, OUTPUT);

  setDin(false);
  parkLeft();
  parkRight();

  state = IDLE;
}

// =====================================================
// LOOP
// =====================================================
void loop() {

  if (!digitalRead(PIN_BTN_ESTOP)) {
    state = ESTOP;
    stateTime = millis();
  }

  switch (state) {

    case IDLE:
      if (!digitalRead(PIN_BTN_DIN_ON)) {
        setDin(true);
        state = DIN_ON;
      }

      if (!digitalRead(PIN_BTN_TEST)) {
        setDin(true);
        state = TEST_RUNNING;
        stateTime = millis();

        if (SERIAL_DEBUG) {
          Serial.println("TEST START");
        }
      }
      break;

    case DIN_ON:
      if (!digitalRead(PIN_BTN_DIN_OFF)) {
        setDin(false);
        state = IDLE;
      }
      break;

    case TEST_RUNNING:
      if (millis() - stateTime >= TEST_DELAY_MS) {

        leftFail  = scanLeft();
        rightFail = scanRight();

        if (SERIAL_DEBUG) {
          Serial.println("TEST END");
        }

        setDin(false);
        state = IDLE;
      }
      break;

    case ESTOP:
      setDin(false);

      if (millis() - stateTime > 5000) {
        state = IDLE;
      }
      break;
  }

  updateLED();
}
