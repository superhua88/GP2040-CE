/*
 * SPDX-License-Identifier: MIT
 * HITBOX WIRELESS RX (dongle, Pico #2) - v5.1 clean pin map (single source of truth)
 *
 * FINAL v5.1 pin plan (same wiring table as TX):
 *   nRF24L01+ SPI0: SCK=GP18, MOSI=GP19, MISO=GP20, CSN=GP21, CE=GP22
 *   Local keys (bench/standalone use): GP02-13, GP16(S1), GP17(S2),
 *     GP26(L3), GP27(R3), GP28(A1), GP15(A2)
 *
 * FIX 2026-10-07 (critical): the previous config defined the radio pins as
 *   NRF24_PIN_SCK/MOSI/... (TX addon names). Nrf24RxAddon reads
 *   NRF24_RX_PIN_SCK/... instead, so the defaults (2/3/4/5/6) were used and
 *   the radio was initialized ON THE BUTTON PINS while the module sits on
 *   GP18-22. Result: no packets ever received, and GP2 was SPI SCK, not a
 *   button ("short GP2 -> no reaction" mystery). Correct macro names now.
 * While a radio packet stream is active the RX addon clearState()s and
 *   rebuilds the gamepad state from packets, so local keys are ignored
 *   during wireless operation (by design). With the TX powered off, the
 *   dongle works as a plain wired hitbox.
 */

#ifndef PICO_BOARD_CONFIG_H_
#define PICO_BOARD_CONFIG_H_

#include "enums.pb.h"
#include "class/hid/hid.h"

#define BOARD_CONFIG_LABEL "Pico RX"

// Local key mapping (same as TX; overridden by radio packets while link is up)
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
#define GPIO_PIN_00 GpioAction::NONE
#define GPIO_PIN_01 GpioAction::NONE
#define GPIO_PIN_14 GpioAction::NONE
#define GPIO_PIN_25 GpioAction::NONE

// nRF24L01+ on SPI0 alternate pins GP18-22.
// NOTE: Nrf24RxAddon reads the NRF24_RX_PIN_* macro names (defaults 2-6!).
#define NRF24_RX_ENABLED 1
#define NRF24_TX_ENABLED 0
#define NRF24_RX_PIN_SCK  18
#define NRF24_RX_PIN_MOSI 19
#define NRF24_RX_PIN_MISO 20
#define NRF24_RX_PIN_CSN  21
#define NRF24_RX_PIN_CE   22

#define PRESS_SIM_PIN 14
#define TURBO_ENABLED 0

#define BATTERY_METER_ENABLED 0
#define METER_LED_PIN 0
#define METER_LED_COUNT 6

// Keyboard Mapping Configuration (unused: no local buttons, keep defaults)
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
