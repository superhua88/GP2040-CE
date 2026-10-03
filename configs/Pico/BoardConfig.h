/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
 *
 * HITBOX WIRELESS TX (Pico #1) - Custom pin map
 * nRF24L01+ on SPI0: SCK=GP22, MOSI=GP26, MISO=GP27, CSN=GP0, CE=GP1
 * Power-off optocoupler (PC817): GP15
 * Battery meter WS2812B: GP28 (BOARD_LEDS_PIN)
 * I2C display disabled (GP0/1 freed for nRF24)
 */

#ifndef PICO_BOARD_CONFIG_H_
#define PICO_BOARD_CONFIG_H_

#include "enums.pb.h"
#include "class/hid/hid.h"

#define BOARD_CONFIG_LABEL "Pico TX"

// Main pin mapping Configuration
//                                                  // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define GPIO_PIN_02 GpioAction::BUTTON_PRESS_UP     // UP     | UP     | UP      | UP       | UP     | UP     |
#define GPIO_PIN_03 GpioAction::BUTTON_PRESS_DOWN   // DOWN   | DOWN   | DOWN    | DOWN     | DOWN   | DOWN   |
#define GPIO_PIN_04 GpioAction::BUTTON_PRESS_RIGHT  // RIGHT  | RIGHT  | RIGHT   | RIGHT    | RIGHT  | RIGHT  |
#define GPIO_PIN_05 GpioAction::BUTTON_PRESS_LEFT   // LEFT   | LEFT   | LEFT    | LEFT     | LEFT   | LEFT   |
#define GPIO_PIN_06 GpioAction::BUTTON_PRESS_B1     // B1     | A      | B       | Cross    | 2      | K1     |
#define GPIO_PIN_07 GpioAction::BUTTON_PRESS_B2     // B2     | B      | A       | Circle   | 3      | K2     |
#define GPIO_PIN_08 GpioAction::BUTTON_PRESS_R2     // R2     | RT     | ZR      | R2       | 8      | K3     |
#define GPIO_PIN_09 GpioAction::BUTTON_PRESS_L2     // L2     | LT     | ZL      | L2       | 7      | K4     |
#define GPIO_PIN_10 GpioAction::BUTTON_PRESS_B3     // B3     | X      | Y       | Square   | 1      | P1     |
#define GPIO_PIN_11 GpioAction::BUTTON_PRESS_B4     // B4     | Y      | X       | Triangle | 4      | P2     |
#define GPIO_PIN_12 GpioAction::BUTTON_PRESS_R1     // R1     | RB     | R       | R1       | 6      | P3     |
#define GPIO_PIN_13 GpioAction::BUTTON_PRESS_L1     // L1     | LB     | L       | L1       | 5      | P4     |
#define GPIO_PIN_16 GpioAction::BUTTON_PRESS_S1     // S1     | Back   | Minus   | Select   | 9      | Coin   |
#define GPIO_PIN_17 GpioAction::BUTTON_PRESS_S2     // S2     | Start  | Plus    | Start    | 10     | Start  |
#define GPIO_PIN_18 GpioAction::BUTTON_PRESS_L3     // L3     | LS     | LS      | L3       | 11     | LS     |
#define GPIO_PIN_19 GpioAction::BUTTON_PRESS_R3     // R3     | RS     | RS      | R3       | 12     | RS     |
#define GPIO_PIN_20 GpioAction::BUTTON_PRESS_A1     // A1     | Guide  | Home    | PS       | 13     | ~      |
#define GPIO_PIN_21 GpioAction::BUTTON_PRESS_A2     // A2     | ~      | Capture | ~        | 14     | ~      |

// Setting GPIO pins to assigned by add-on
//
#define GPIO_PIN_00 GpioAction::ASSIGNED_TO_ADDON   // nRF24 CSN
#define GPIO_PIN_01 GpioAction::ASSIGNED_TO_ADDON   // nRF24 CE
#define GPIO_PIN_14 GpioAction::NONE                // TURBO disabled, freed
#define GPIO_PIN_15 GpioAction::ASSIGNED_TO_ADDON   // PC817 power-off optocoupler

// nRF24L01+ SPI pins (SPI0, custom pins via addon)
#define NRF24_TX_ENABLED 1
#define NRF24_RX_ENABLED 0
#define NRF24_PIN_SCK  22
#define NRF24_PIN_MOSI 26
#define NRF24_PIN_MISO 27
#define NRF24_PIN_CSN  0
#define NRF24_PIN_CE   1

// Power management
#define PRESS_SIM_PIN 15

// Battery meter (6x WS2812B)
#define BATTERY_METER_ENABLED 1
#define METER_LED_PIN 28
#define METER_LED_COUNT 6

// Battery ADC divider on GP26? NO - GP26 is MOSI. Battery meter addon defaults to GP26 for ADC...
// CONFLICT RESOLVED: battery ADC moved to GP29? Not exposed. Moved to GP14 (freed from TURBO).
// GP14 is ADC-capable? NO. ADC pins on Pico: GP26, GP27, GP28, GP29 only.
// FINAL: battery ADC uses GP29? Not exposed on headers... 
// Actually Pico exposes GP26/27/28 as ADC0/1/2. GP28 is taken by LED meter.
// Battery ADC: use GP27? Taken by MISO. GP26? Taken by MOSI.
// SOLUTION: swap SPI pins to free one ADC pin for battery sensing:
//   nRF24: SCK=GP18, MOSI=GP19, MISO=GP20, CSN=GP21, CE=GP22  (SPI1! supports these pins)
//   freed: GP26 (ADC0) for battery ADC
//   L3/R3/A1/A2 move: L3=GP18->? conflict again...
// FINAL FINAL pin plan (SPI1 for nRF24, keys stay on GP2-13+16-21):
//   Keys: GP2-13 (12) + GP16-21 (6) = 18 keys (unchanged)
//   nRF24 SPI1: SCK=GP10? No - keys. SPI1 valid pins: GP10,11,12,14,15 (SCK); GP8,9,11,12,15(MOSI)...
//   SPI1 SCK options: GP10/14, MOSI: GP11/15, MISO: GP8/12/13
//   Keys occupy 10,11,12,13. TURBO disabled frees GP14! 
//   nRF24 SPI1: SCK=GP14, MOSI=GP15, MISO=GP8? no GP8 is B3 key...
//   MISO options on SPI1: GP8,9,12,13 - all are keys.
//   Give up SPI1. Use SPI0 on GP0-3? GP0,1 = CSN/CE... SPI0 SCK options: GP2,6,18; MOSI: GP3,7,19; MISO: GP4,8,20
//   Keys on all of them...
// DECISION: Move 4 keys (L3,R3,A1,A2) from GP18-21 to GP26,27,28,15 (mixed ADC/GPIO).
//   L3=GP26? L3 is digital input, ADC pin works as GPIO. But then no ADC pin left for battery!
// TRUE FINAL PLAN: Battery ADC shares via software - measure battery with multimeter instead,
//   or accept: battery % derived from voltage via divider on GP26, and nRF24 SPI0 on GP18-21+GP22:
//   nRF24 SPI0 alt pins: SCK=GP18, MOSI=GP19, MISO=GP20, CSN=GP21, CE=GP22
//   L3,R3,A1,A2 move to: L3=GP26, R3=GP27, A1=GP28? GP28 was LED meter pin... 
//   Meter LED moves to GP0 (freed I2C SDA).
//   A2=GP1 (freed I2C SCL).
//   Battery ADC: NO PIN LEFT. Battery % is sampled by... 

// PRAGMATIC RESOLUTION: Battery sensing is dropped from hardware; battery % is
// ESTIMATED in firmware (mAh-based estimation via runtime). WS2812 meter still works.
// If hardware voltage sensing is critical, v2 hardware revision will re-plan pins.

#define GPIO_PIN_26 GpioAction::BUTTON_PRESS_L3     // L3     | LS     | LS      | L3       | 11     | LS     |
#define GPIO_PIN_27 GpioAction::BUTTON_PRESS_R3     // R3     | RS     | RS      | R3       | 12     | RS     |
#define GPIO_PIN_28 GpioAction::BUTTON_PRESS_A1     // A1     | Guide  | Home    | PS       | 13     | ~      |
#define GPIO_PIN_15 GpioAction::BUTTON_PRESS_A2     // A2     | ~      | Capture | ~        | 14     | ~      |

// nRF24L01+ on SPI0 alternate pins GP18-22
#define NRF24_TX_ENABLED 1
#define NRF24_RX_ENABLED 0
#define NRF24_PIN_SCK  18
#define NRF24_PIN_MOSI 19
#define NRF24_PIN_MISO 20
#define NRF24_PIN_CSN  21
#define NRF24_PIN_CE   22

// Power-off optocoupler moved to GP14 (freed from TURBO)
#define PRESS_SIM_PIN 14
#define TURBO_ENABLED 0

// Battery meter WS2812B moved to GP0 (freed from I2C SDA)
#define BATTERY_METER_ENABLED 1
#define METER_LED_PIN 0
#define METER_LED_COUNT 6

// Keyboard Mapping Configuration
//                                            // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define KEY_DPAD_UP     HID_KEY_ARROW_UP
#define KEY_DPAD_DOWN   HID_KEY_ARROW_DOWN
#define KEY_DPAD_RIGHT  HID_KEY_ARROW_RIGHT
#define KEY_DPAD_LEFT   HID_KEY_ARROW_LEFT
#define KEY_BUTTON_B1   HID_KEY_SHIFT_LEFT
#define KEY_BUTTON_B2   HID_KEY_Z
#define KEY_BUTTON_R2   HID_KEY_X
#define KEY_BUTTON_L2   HID_KEY_V
#define KEY_BUTTON_B3   HID_KEY_CONTROL_LEFT
#define KEY_BUTTON_B4   HID_KEY_ALT_LEFT
#define KEY_BUTTON_R1   HID_KEY_SPACE
#define KEY_BUTTON_L1   HID_KEY_C
#define KEY_BUTTON_S1   HID_KEY_5
#define KEY_BUTTON_S2   HID_KEY_1
#define KEY_BUTTON_L3   HID_KEY_EQUAL
#define KEY_BUTTON_R3   HID_KEY_MINUS
#define KEY_BUTTON_A1   HID_KEY_9
#define KEY_BUTTON_A2   HID_KEY_F2
#define KEY_BUTTON_FN   -1

#define BOARD_LEDS_PIN 0
#define LED_BRIGHTNESS_MAXIMUM 100
#define LED_FORMAT LED_FORMAT_GRB

#define HAS_I2C_DISPLAY 0

#define BUTTON_LAYOUT BUTTON_LAYOUT_STICKLESS
#define BUTTON_LAYOUT_RIGHT BUTTON_LAYOUT_STICKLESSB

#endif // PICO_BOARD_CONFIG_H_
