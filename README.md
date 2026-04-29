# Tester Firmware – Analog Switch (MP4835AGRD)

## Overview

This project is a firmware for an automated analog switch testing system based on Arduino Mega.
It is designed to validate a large number of analog switch channels using MUX scanning and ADC-based verification.

The system controls multiple sub boards and detects failures by analyzing analog voltage levels from monitoring channels.

---

## Key Features

* Multi-channel analog switch testing (6000+ channels)
* MUX-based channel scanning (Address + Bank selection)
* ADC-based pass/fail detection
* Left / Right bank independent verification
* Hardware button control (DIN ON / TEST / ESTOP)
* LED status indication (Active-Low control)
* Serial debug logging for failure analysis

---

## System Architecture

### Test Flow

1. Enable DIN (Digital Input)
2. Wait for stabilization
3. Scan all channels using MUX
4. Read ADC values
5. Compare with threshold
6. Report FAIL channels via Serial
7. Return to IDLE state

---

## Hardware Configuration

### MCU

* Arduino Mega 2560

### Interfaces

* MUX Address Lines: S0 ~ S3
* Bank Select (Left / Right): 3-bit selection each
* ADC Input: A0 (monitoring channel)

### Control Signals

* DIN Buffers (4 channels)
* Push Buttons:

  * DIN ON
  * DIN OFF
  * TEST
  * ESTOP

### Status LEDs (Active Low)

* IDLE
* DIN ON
* TEST RUNNING
* LEFT FAIL
* RIGHT FAIL
* ESTOP

---

## Pin Mapping (Summary)

| Function          | Pins    |
| ----------------- | ------- |
| MUX Address       | 22 ~ 25 |
| Left Bank Select  | 26 ~ 28 |
| Right Bank Select | 29 ~ 31 |
| DIN Control       | 32 ~ 35 |
| Buttons           | 38 ~ 41 |
| LEDs              | 42 ~ 47 |
| ADC               | A0      |

---

## Test Logic

### Channel Calculation

* Channel = Bank * 16 + Address

### ADC Evaluation

* Left Threshold: adcOkMaxLeft = 150
* Right Threshold: adcOkMaxRight = 250

If ADC value exceeds threshold, the channel is marked as FAIL.

---

## Serial Output Example

```
TEST START
[FAIL][LEFT] CH=10 (BANK=0 ADDR=10) ADC=151
[FAIL][RIGHT] CH=41 (BANK=2 ADDR=9) ADC=260
TEST END
```

---

## State Machine

* INIT
* IDLE
* DIN_ON
* TEST_RUNNING
* ESTOP

### Behavior Summary

* IDLE: Wait for input
* DIN_ON: Force DIN active
* TEST_RUNNING: Execute full scan
* ESTOP: Immediate shutdown (5-second hold)

---

## Timing Parameters

| Parameter       | Value               |
| --------------- | ------------------- |
| Test Delay      | 12,000 ms           |
| MUX Settle Time | 10,000 µs           |
| ADC Sampling    | 8 samples averaging |

---

## Implementation Highlights

* Noise reduction using ADC averaging
* MUX settling delay for stable measurement
* Bank isolation (Left / Right parking)
* Active-Low LED control logic
* Modular low-level control functions (MUX, DIN, ADC)

---

## Future Improvements

* Refactor into C++ modular structure (.cpp / .h)
* Add UART command interface (PC control)
* Logging system (SD card or external interface)
* Dynamic threshold calibration
* GUI-based monitoring tool (Python)

---

## Development Environment

* Arduino IDE
* Target Board: Arduino Mega 2560
* Baud Rate: 115200

---

## Author Notes

This project was developed as part of a large-scale hardware validation system for testing analog switch ICs in a multi-board environment.

It demonstrates practical embedded system design including:

* Hardware control
* Signal integrity considerations
* Automated test logic
* Fault detection system

---
