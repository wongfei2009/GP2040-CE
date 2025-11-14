# Sky 2040 Version 2 Board Configuration

## ⚠️ Hardware Verification Required

**STATUS**: This configuration contains **PLACEHOLDER GPIO MAPPINGS** that must be verified with the physical Sky 2040 v2 board before use.

All pin assignments marked with `[TO BE VERIFIED]` in `BoardConfig.h` need to be tested and updated with actual GPIO numbers once the hardware is available.

---

## Board Information

- **Form Factor**: Raspberry Pi Pico compatible
- **MCU**: RP2040
- **Board Version**: Sky 2040 Version 2

## Features

- 18-button arcade layout (D-Pad, 8 face buttons, 6 shoulder/system buttons, 2 aux)
- I2C OLED display support
- RGB LED support (16 LEDs)
- Hardware turbo via MCP23017 I2C GPIO expander
- Analog turbo speed dial (potentiometer)

---

## Pin Verification Checklist

When the physical board arrives, verify and update the following in `BoardConfig.h`:

### 1. Button Pin Mappings
- [ ] D-Pad: UP, DOWN, LEFT, RIGHT (GPIO_PIN_02-05)
- [ ] Face Buttons: B1, B2, B3, B4 (GPIO_PIN_06-07, 10-11)
- [ ] Shoulder Buttons: L1, L2, R1, R2 (GPIO_PIN_08-09, 12-13)
- [ ] System Buttons: S1, S2 (GPIO_PIN_16-17)
- [ ] Stick Buttons: L3, R3 (GPIO_PIN_18-19)
- [ ] Aux Buttons: A1, A2 (GPIO_PIN_20-21)
- [ ] Turbo Button: GPIO_PIN_14

### 2. I2C Configuration
- [ ] I2C SDA pin (currently: GP0)
- [ ] I2C SCL pin (currently: GP1)
- [ ] Display I2C address (usually 0x3C for OLED)
- [ ] MCP23017 I2C address (currently: 0x20)

### 3. RGB LED Configuration
- [ ] LED data pin (currently: GP28)
- [ ] LED count and layout mapping
- [ ] LED format: GRB vs RGB vs other

### 4. Turbo Hardware
- [ ] Turbo LED indicator pin (currently: GP15)
- [ ] Turbo speed dial ADC pin (currently: GP26)
- [ ] Verify ADC pin is actually ADC-capable (GP26-GP29 on RP2040)

---

## Hardware Turbo Enhancement

This configuration includes support for hardware-based turbo controls:

### Components Required
- **MCP23017 I2C GPIO Expander breakout board**
- 8× SPST toggle switches (or 1× DIP-8 switch array)
- 10kΩ linear potentiometer (rotary or slide)
- Breadboard and jumper wires (for prototyping)

### Features
- **Per-button turbo switches**: 8 independent switches for B1-B4, L1-L2, R1-R2
- **Analog turbo speed dial**: Adjustable turbo rate (2-30 shots/sec)
- **I2C bus sharing**: MCP23017 shares I2C bus with display (no extra pins needed)

### MCP23017 Wiring
| MCP23017 Pin | Connection | Notes |
|--------------|------------|-------|
| VDD/VCC | 3.3V | Power |
| GND | GND | Ground |
| SDA | GP0 | I2C Data (verify pin) |
| SCL | GP1 | I2C Clock (verify pin) |
| A0-A2 | GND | I2C address 0x20 (default) |
| GPA0-GPA7 | Switches | Connect switches between pin and GND |

### Turbo Switch Mapping (MCP23017 Port A)
| MCP Pin | Button | Switch Function |
|---------|--------|-----------------|
| GPA0 | B1 | Turbo for B1 (Cross/A) |
| GPA1 | B2 | Turbo for B2 (Circle/B) |
| GPA2 | B3 | Turbo for B3 (Square/X) |
| GPA3 | B4 | Turbo for B4 (Triangle/Y) |
| GPA4 | L1 | Turbo for L1 (LB) |
| GPA5 | R1 | Turbo for R1 (RB) |
| GPA6 | L2 | Turbo for L2 (LT) |
| GPA7 | R2 | Turbo for R2 (RT) |

### Speed Dial Wiring
| Potentiometer Pin | Connection | Notes |
|-------------------|------------|-------|
| Pin 1 | GND | Ground |
| Pin 2 (wiper) | GP26 | ADC input (verify pin) |
| Pin 3 | 3.3V | Reference voltage |

---

## Build Instructions

### Prerequisites
- ARM GCC toolchain installed via Homebrew: `brew install --cask gcc-arm-embedded`
- Pico SDK 2.1.1 cloned to `pico-sdk-2.1.1/` in project root
- macOS SDK path set: `export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)`

### Build Commands
```bash
# From project root directory
rm -rf build && mkdir build && cd build

# Configure for Sky2040v2
PICO_SDK_PATH=$(pwd)/../pico-sdk-2.1.1 GP2040_BOARDCONFIG=Sky2040v2 cmake -DPICO_SDK_FETCH_FROM_GIT=OFF ..

# Build
make -j$(sysctl -n hw.ncpu)

# Output: GP2040-CE_X.X.X_Sky2040v2.uf2
```

### Flashing
1. Hold BOOTSEL button while plugging in USB (or short RUN to GND twice)
2. Device appears as `RPI-RP2` drive
3. Copy `.uf2` file to the drive
4. Device reboots automatically with new firmware

---

## Testing Procedure

After updating pin mappings and building firmware:

1. **Basic Functionality Test**
   - [ ] All 18 buttons register correctly
   - [ ] I2C display initializes and shows status
   - [ ] RGB LEDs light up and respond to buttons

2. **Turbo Hardware Test** (if MCP23017 installed)
   - [ ] Each of 8 turbo switches controls correct button
   - [ ] Speed dial adjusts turbo rate smoothly
   - [ ] Display and MCP23017 coexist without I2C conflicts
   - [ ] No added input latency (<1ms overhead)

3. **Cross-Platform Test**
   - [ ] PC (XInput mode)
   - [ ] Nintendo Switch
   - [ ] PS4/PS5 (if available)

---

## I2C Address Reference

| Device | Address | Notes |
|--------|---------|-------|
| OLED Display | 0x3C | Standard SSD1306 address |
| MCP23017 GPIO Expander | 0x20 | Default address (A0-A2 tied to GND) |

**Important**: The MCP23017 address (0x20) does not conflict with the display (0x3C), so both can safely share the I2C bus.

---

## Troubleshooting

### I2C Issues
- Verify SDA/SCL pin assignments match physical board
- Check I2C bus speed (400kHz standard)
- Confirm MCP23017 address configuration (should be 0x20)
- Ensure display initializes before MCP23017 (display addon loads first)

### Turbo Not Working
- Verify switches wired correctly (active-low: switch between MCP pin and GND)
- Check MCP23017 I2C address (should respond at 0x20)
- Confirm turbo speed dial on ADC-capable pin (GP26-GP29)
- Ensure `TURBO_I2C_SWITCHES_ENABLED` is defined in BoardConfig.h

### Build Errors
- Ensure Pico SDK 2.1.1 is in `pico-sdk-2.1.1/` directory
- Run `git submodule update --init --recursive` in SDK directory
- Set `SDKROOT` environment variable on macOS
- Clean build directory and retry: `rm -rf build`

---

## Documentation References

- [GP2040-CE Documentation](https://gp2040-ce.info)
- [Turbo Add-on Guide](https://gp2040-ce.info/add-ons/turbo/)
- [MCP23017 Datasheet](https://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf)
- [Project Notes](../../CLAUDE.md)

---

**Last Updated**: 2024-11-14  
**Status**: Awaiting hardware for GPIO verification
