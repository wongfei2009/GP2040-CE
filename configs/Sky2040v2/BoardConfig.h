/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
 */

#ifndef SKY2040V2_BOARD_CONFIG_H_
#define SKY2040V2_BOARD_CONFIG_H_

#include "enums.pb.h"
#include "class/hid/hid.h"

#define BOARD_CONFIG_LABEL "Sky2040v2"

// ============================================================================
// NOTE: GPIO pin mappings below are PLACEHOLDERS and must be verified with
// the physical Sky 2040 v2 board. Update all GPIO_PIN_XX assignments with
// actual pin numbers once hardware is available for testing.
// ============================================================================

// Main pin mapping Configuration
//                                                  // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define GPIO_PIN_02 GpioAction::BUTTON_PRESS_UP     // UP     | UP     | UP      | UP       | UP     | UP     | [TO BE VERIFIED]
#define GPIO_PIN_03 GpioAction::BUTTON_PRESS_DOWN   // DOWN   | DOWN   | DOWN    | DOWN     | DOWN   | DOWN   | [TO BE VERIFIED]
#define GPIO_PIN_04 GpioAction::BUTTON_PRESS_RIGHT  // RIGHT  | RIGHT  | RIGHT   | RIGHT    | RIGHT  | RIGHT  | [TO BE VERIFIED]
#define GPIO_PIN_05 GpioAction::BUTTON_PRESS_LEFT   // LEFT   | LEFT   | LEFT    | LEFT     | LEFT   | LEFT   | [TO BE VERIFIED]
#define GPIO_PIN_06 GpioAction::BUTTON_PRESS_B1     // B1     | A      | B       | Cross    | 2      | K1     | [TO BE VERIFIED]
#define GPIO_PIN_07 GpioAction::BUTTON_PRESS_B2     // B2     | B      | A       | Circle   | 3      | K2     | [TO BE VERIFIED]
#define GPIO_PIN_08 GpioAction::BUTTON_PRESS_R2     // R2     | RT     | ZR      | R2       | 8      | K3     | [TO BE VERIFIED]
#define GPIO_PIN_09 GpioAction::BUTTON_PRESS_L2     // L2     | LT     | ZL      | L2       | 7      | K4     | [TO BE VERIFIED]
#define GPIO_PIN_10 GpioAction::BUTTON_PRESS_B3     // B3     | X      | Y       | Square   | 1      | P1     | [TO BE VERIFIED]
#define GPIO_PIN_11 GpioAction::BUTTON_PRESS_B4     // B4     | Y      | X       | Triangle | 4      | P2     | [TO BE VERIFIED]
#define GPIO_PIN_12 GpioAction::BUTTON_PRESS_R1     // R1     | RB     | R       | R1       | 6      | P3     | [TO BE VERIFIED]
#define GPIO_PIN_13 GpioAction::BUTTON_PRESS_L1     // L1     | LB     | L       | L1       | 5      | P4     | [TO BE VERIFIED]
#define GPIO_PIN_16 GpioAction::BUTTON_PRESS_S1     // S1     | Back   | Minus   | Select   | 9      | Coin   | [TO BE VERIFIED]
#define GPIO_PIN_17 GpioAction::BUTTON_PRESS_S2     // S2     | Start  | Plus    | Start    | 10     | Start  | [TO BE VERIFIED]
#define GPIO_PIN_18 GpioAction::BUTTON_PRESS_L3     // L3     | LS     | LS      | L3       | 11     | LS     | [TO BE VERIFIED]
#define GPIO_PIN_19 GpioAction::BUTTON_PRESS_R3     // R3     | RS     | RS      | R3       | 12     | RS     | [TO BE VERIFIED]
#define GPIO_PIN_20 GpioAction::BUTTON_PRESS_A1     // A1     | Guide  | Home    | PS       | 13     | ~      | [TO BE VERIFIED]
#define GPIO_PIN_21 GpioAction::BUTTON_PRESS_A2     // A2     | ~      | Capture | ~        | 14     | ~      | [TO BE VERIFIED]

// Setting GPIO pins to assigned by add-on
// NOTE: Verify these pin assignments with physical board
#define GPIO_PIN_00 GpioAction::ASSIGNED_TO_ADDON   // I2C SDA (shared with display) [TO BE VERIFIED]
#define GPIO_PIN_01 GpioAction::ASSIGNED_TO_ADDON   // I2C SCL (shared with display) [TO BE VERIFIED]
#define GPIO_PIN_15 GpioAction::ASSIGNED_TO_ADDON   // Turbo LED [TO BE VERIFIED]
#define GPIO_PIN_28 GpioAction::ASSIGNED_TO_ADDON   // RGB LEDs [TO BE VERIFIED]

// Keyboard Mapping Configuration
//                                            // GP2040 | Xinput | Switch  | PS3/4/5  | Dinput | Arcade |
#define KEY_DPAD_UP     HID_KEY_ARROW_UP      // UP     | UP     | UP      | UP       | UP     | UP     |
#define KEY_DPAD_DOWN   HID_KEY_ARROW_DOWN    // DOWN   | DOWN   | DOWN    | DOWN     | DOWN   | DOWN   |
#define KEY_DPAD_RIGHT  HID_KEY_ARROW_RIGHT   // RIGHT  | RIGHT  | RIGHT   | RIGHT    | RIGHT  | RIGHT  |
#define KEY_DPAD_LEFT   HID_KEY_ARROW_LEFT    // LEFT   | LEFT   | LEFT    | LEFT     | LEFT   | LEFT   |
#define KEY_BUTTON_B1   HID_KEY_SHIFT_LEFT    // B1     | A      | B       | Cross    | 2      | K1     |
#define KEY_BUTTON_B2   HID_KEY_Z             // B2     | B      | A       | Circle   | 3      | K2     |
#define KEY_BUTTON_R2   HID_KEY_X             // R2     | RT     | ZR      | R2       | 8      | K3     |
#define KEY_BUTTON_L2   HID_KEY_V             // L2     | LT     | ZL      | L2       | 7      | K4     |
#define KEY_BUTTON_B3   HID_KEY_CONTROL_LEFT  // B3     | X      | Y       | Square   | 1      | P1     |
#define KEY_BUTTON_B4   HID_KEY_ALT_LEFT      // B4     | Y      | X       | Triangle | 4      | P2     |
#define KEY_BUTTON_R1   HID_KEY_SPACE         // R1     | RB     | R       | R1       | 6      | P3     |
#define KEY_BUTTON_L1   HID_KEY_C             // L1     | LB     | L       | L1       | 5      | P4     |
#define KEY_BUTTON_S1   HID_KEY_5             // S1     | Back   | Minus   | Select   | 9      | Coin   |
#define KEY_BUTTON_S2   HID_KEY_1             // S2     | Start  | Plus    | Start    | 10     | Start  |
#define KEY_BUTTON_L3   HID_KEY_EQUAL         // L3     | LS     | LS      | L3       | 11     | LS     |
#define KEY_BUTTON_R3   HID_KEY_MINUS         // R3     | RS     | RS      | R3       | 12     | RS     |
#define KEY_BUTTON_A1   HID_KEY_9             // A1     | Guide  | Home    | PS       | 13     | ~      |
#define KEY_BUTTON_A2   HID_KEY_F2            // A2     | ~      | Capture | ~        | 14     | ~      |
#define KEY_BUTTON_FN   -1                    // Hotkey Function                                        |

// Turbo Configuration
#define TURBO_ENABLED 1
#define GPIO_PIN_14 GpioAction::BUTTON_PRESS_TURBO  // [TO BE VERIFIED]
#define TURBO_LED_PIN 15                             // [TO BE VERIFIED]

// Turbo Speed Dial (Analog Potentiometer)
// NOTE: Must be an ADC-capable pin (GP26-GP29 on RP2040)
#define PIN_SHMUP_DIAL 26                            // [TO BE VERIFIED]

// I2C Turbo Switches Configuration (MCP23017 GPIO Expander)
#define TURBO_I2C_SWITCHES_ENABLED 1
#define TURBO_I2C_SDA_PIN 0        // Shared with display [TO BE VERIFIED]
#define TURBO_I2C_SCL_PIN 1        // Shared with display [TO BE VERIFIED]
#define TURBO_I2C_BLOCK i2c0       // Same I2C block as display
#define TURBO_I2C_SPEED 400000     // 400kHz (standard for I2C displays)
#define TURBO_I2C_ADDR 0x20        // MCP23017 default address (no conflict with display at 0x3C)

// RGB LED Configuration
#define BOARD_LEDS_PIN 28                            // [TO BE VERIFIED]
#define LED_BRIGHTNESS_MAXIMUM 100
#define LED_BRIGHTNESS_STEPS 5
#define LED_FORMAT LED_FORMAT_GRB                    // [TO BE VERIFIED - may be LED_FORMAT_RGB]
#define LEDS_PER_PIXEL 1

// LED Position Mapping (assumes 16 RGB LEDs in standard arcade layout)
#define LEDS_DPAD_LEFT   0
#define LEDS_DPAD_DOWN   1
#define LEDS_DPAD_RIGHT  2
#define LEDS_DPAD_UP     3
#define LEDS_BUTTON_B3   4
#define LEDS_BUTTON_B4   5
#define LEDS_BUTTON_R1   6
#define LEDS_BUTTON_L1   7
#define LEDS_BUTTON_B1   8
#define LEDS_BUTTON_B2   9
#define LEDS_BUTTON_R2   10
#define LEDS_BUTTON_L2   11
#define LEDS_BUTTON_A1   12
#define LEDS_BUTTON_L3   13
#define LEDS_BUTTON_R3   14
#define LEDS_BUTTON_A2   15

// I2C Display Configuration
#define HAS_I2C_DISPLAY 1
#define I2C0_ENABLED 1
#define I2C0_PIN_SDA 0                               // [TO BE VERIFIED]
#define I2C0_PIN_SCL 1                               // [TO BE VERIFIED]
#define DISPLAY_I2C_BLOCK i2c0

// Button Layout (for web configurator and display)
#define BUTTON_LAYOUT BUTTON_LAYOUT_STICKLESS
#define BUTTON_LAYOUT_RIGHT BUTTON_LAYOUT_STICKLESSB

#endif
