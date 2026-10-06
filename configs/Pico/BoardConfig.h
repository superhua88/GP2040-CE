/*
 * SPDX-License-Identifier: MIT
 * HITBOX WIRELESS TX (Pico #1) - v5.1 clean pin map (single source of truth)
 *
 * FINAL v5.1 pin plan:
 *   nRF24L01+ SPI0: SCK=GP18, MOSI=GP19, MISO=GP20, CSN=GP21, CE=GP22
 *   Keys: GP02-13 (12) + GP16(S1) GP17(S2) + GP26(L3) GP27(R3) GP28(A1) GP15(A2)
 *   Power-off optocoupler (PC817): GP14 (PRESS_SIM_PIN)
 *   Battery meter WS2812B x6: GP0 (METER_LED_PIN)
 *   Battery %: firmware estimation (no ADC pin left on header)
 *
 * FIX 2026-10-07: removed the stale duplicate block that mapped
 *   L3/R3/A1/A2 onto GP18/19/20/21 (same pins as the nRF24 SPI wires).
 *   SPI idle-low levels were read as pressed buttons -> phantom LS+RS on
 *   XInput, and the phantom bits were broadcast in every radio packet.
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
#define GPIO_PIN_26 GpioAction::BUTTON_PRESS_L3     // L3     | LS     | LS      | L3       | 11     | LS     |
#define GPIO_PIN_27 GpioAction::BUTTON_PRESS_R3     // R3     | RS     | RS      | R3       | 12     | RS     |
#define GPIO_PIN_28 GpioAction::BUTTON_PRESS_A1     // A1     | Guide  | Home    | PS       | 13     | ~      |
#define GPIO_PIN_15 GpioAction::BUTTON_PRESS_A2     // A2     | ~      | Capture | ~        | 14     | ~      |

// Pins owned by the nRF24 radio - must NOT be mapped as buttons
#define GPIO_PIN_18 GpioAction::NONE                // nRF24 SCK  (SPI0)
#define GPIO_PIN_19 GpioAction::NONE                // nRF24 MOSI (SPI0)
#define GPIO_PIN_20 GpioAction::NONE                // nRF24 MISO (SPI0)
#define GPIO_PIN_21 GpioAction::NONE                // nRF24 CSN  (SPI0)
#define GPIO_PIN_22 GpioAction::NONE                // nRF24 CE
#define GPIO_PIN_00 GpioAction::ASSIGNED_TO_ADDON   // WS2812B battery meter
#define GPIO_PIN_01 GpioAction::NONE                // spare
#define GPIO_PIN_14 GpioAction::NONE                // PC817 power-off pulse (driven by TX addon)
#define GPIO_PIN_25 GpioAction::NONE                // spare (Pico onboard LED unused)

// nRF24L01+ on SPI0 alternate pins GP18-22 (single definition, v5 plan)
#define NRF24_TX_ENABLED 1
#define NRF24_RX_ENABLED 0
#define NRF24_PIN_SCK  18
#define NRF24_PIN_MOSI 19
#define NRF24_PIN_MISO 20
#define NRF24_PIN_CSN  21
#define NRF24_PIN_CE   22

// Power-off optocoupler on GP14 (freed from TURBO)
#define PRESS_SIM_PIN 14
#define TURBO_ENABLED 0

// Battery meter WS2812B on GP0 (freed from I2C)
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
