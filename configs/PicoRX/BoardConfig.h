/*
 * SPDX-License-Identifier: MIT
 * HITBOX WIRELESS RX (dongle, Pico #2) - nRF24 receiver + USB gamepad
 * Keys remain mapped (they read as floating/neutral), used only if standalone.
 */

#ifndef PICO_BOARD_CONFIG_H_
#define PICO_BOARD_CONFIG_H_

#include "enums.pb.h"
#include "class/hid/hid.h"

#define BOARD_CONFIG_LABEL "Pico RX"

// Same key mapping as TX (local keys read neutral while wireless active)
#define GPIO_PIN_02 GpioAction::BUTTON_PRESS_UP
#define GPIO_PIN_03 GpioAction::BUTTON_PRESS_DOWN
#define GPIO_PIN_04 GpioAction::BUTTON_PRESS_RIGHT
#define GPIO_PIN_05 GpioAction::BUTTON_PRESS_LEFT
#define GPIO_PIN_06 GpioAction::BUTTON_PRESS_B1
#define GPIO_PIN_07 GpioAction::BUTTON_PRESS_B2
#define GPIO_PIN_08 GpioAction::BUTTON_PRESS_R2
#define GPIO_PIN_09 GpioAction::BUTTON_PRESS_L2
#define GPIO_PIN_10 GpioAction::BUTTON_PRESS_B3
#define GPIO_PIN_11 GpioAction::BUTTON_PRESS_B4
#define GPIO_PIN_12 GpioAction::BUTTON_PRESS_R1
#define GPIO_PIN_13 GpioAction::BUTTON_PRESS_L1
#define GPIO_PIN_16 GpioAction::BUTTON_PRESS_S1
#define GPIO_PIN_17 GpioAction::BUTTON_PRESS_S2
#define GPIO_PIN_18 GpioAction::BUTTON_PRESS_L3
#define GPIO_PIN_19 GpioAction::BUTTON_PRESS_R3
#define GPIO_PIN_20 GpioAction::BUTTON_PRESS_A1
#define GPIO_PIN_21 GpioAction::BUTTON_PRESS_A2

#define GPIO_PIN_00 GpioAction::ASSIGNED_TO_ADDON
#define GPIO_PIN_01 GpioAction::ASSIGNED_TO_ADDON
#define GPIO_PIN_14 GpioAction::NONE
#define GPIO_PIN_15 GpioAction::ASSIGNED_TO_ADDON

// nRF24 SPI0 alt pins (same as TX so the wiring table is identical)
#define NRF24_RX_ENABLED 1
#define NRF24_TX_ENABLED 0
#define NRF24_PIN_SCK  18
#define NRF24_PIN_MOSI 19
#define NRF24_PIN_MISO 20
#define NRF24_PIN_CSN  21
#define NRF24_PIN_CE   22

#define PRESS_SIM_PIN 14
#define TURBO_ENABLED 0

#define BATTERY_METER_ENABLED 0
#define METER_LED_PIN 0
#define METER_LED_COUNT 6

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

#define HAS_I2C_DISPLAY 0
#define BUTTON_LAYOUT BUTTON_LAYOUT_STICKLESS
#define BUTTON_LAYOUT_RIGHT BUTTON_LAYOUT_STICKLESSB

#endif // PICO_BOARD_CONFIG_H_
