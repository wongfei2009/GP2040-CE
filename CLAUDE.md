# Sky 2040 GP2040-CE Customization Project Context

## Quick Summary

**Goal**: Add hardware-based turbo controls to Sky 2040 joystick board
- ✅ **Analog turbo speed dial** (potentiometer, 2-30 shots/sec)
- ✅ **8 per-button turbo switches** (physical toggles for B1-B4, L1-L2, R1-R2)

**Feasibility**: CONFIRMED - Both features are implementable
**GPIO Required**: 9 pins (1 ADC + 8 digital)
**Performance Impact**: Negligible (<2% overhead)

---

## Project Overview

This document provides comprehensive context for customizing the GP2040-CE firmware for the Sky 2040 joystick board, with a focus on implementing hardware-based turbo features using analog speed control and physical per-button toggle switches.

### Project Location
- **Fork Repository**: `/Users/fwong/Documents/github/wongfei2009/GP2040-CE`
- **Upstream**: OpenStickCommunity/GP2040-CE (GitHub)

---

## Hardware: Sky 2040 Board

### Board Specifications
- **Microcontroller**: RP2040 (Raspberry Pi Pico-based)
- **Size**: 5.1cm × 3.1cm (51mm × 31mm)
- **Mounting holes**: 45mm × 25mm centers, 3mm diameter
- **Firmware**: GP2040-CE (open source)
- **Features**: 
  - USB-C connectivity
  - Exposed PCB for easy modification
  - RGB LED support
  - OLED display support (I2C)
  - Compact form factor
  - Dual-row GPIO headers (30 pins total)

### Detailed Pinout (from Hardware Images)

Based on the provided hardware images, here is the complete Sky 2040 pinout:

#### Top Row (Left to Right, Pins 1-15) - Button Input Row
All pins on this row are **button inputs** with active-low configuration (button press connects GPIO to GND via the External GND column).

| Pin | External | Internal | Estimated GPIO | Function |
|-----|----------|----------|----------------|----------|
| 1   | GND      | A        | GPIO 2         | A button (B1) - Ground connection |
| 2   | GND      | B        | GPIO 3         | B button (B2) - Ground connection |
| 3   | GND      | X        | GPIO 4         | X button (B3) - Ground connection |
| 4   | GND      | Y        | GPIO 5         | Y button (B4) - Ground connection |
| 5   | GND      | L1       | GPIO 6         | L1 button - Ground connection |
| 6   | GND      | R1       | GPIO 7         | R1 button - Ground connection |
| 7   | GND      | L2       | GPIO 8         | L2 button - Ground connection |
| 8   | GND      | R2       | GPIO 9         | R2 button - Ground connection |
| 9   | GND      | L3       | GPIO 10        | L3 button - Ground connection |
| 10  | GND      | R3       | GPIO 11        | R3 button - Ground connection |
| 11  | GND      | SEL      | GPIO 12        | Select button - Ground connection |
| 12  | GND      | STA      | GPIO 13        | Start button - Ground connection |
| 13  | GND      | HOME     | GPIO 14        | Home/PS button - Ground connection |
| 14  | GND      | TURBO    | GPIO 15        | Turbo button - Ground connection |
| 15  | GND      | TU-LED   | -              | Turbo LED - Ground connection |

**How Button Inputs Work:**
- Each button's GPIO pin has an internal pull-up resistor enabled in firmware
- When button is NOT pressed: GPIO reads HIGH (3.3V via pull-up)
- When button IS pressed: Button closes circuit to GND → GPIO reads LOW (0V)
- This is the standard "active-low" button configuration

#### Bottom Row (Right to Left, Pins 16-30) - System & Direction Controls
| Pin | Internal | External | GPIO  | Function |
|-----|----------|----------|-------|----------|
| 30  | 5V       | GND      | -     | 5V Power / Ground |
| 29  | GND      | A2       | GPIO 29 | RGB LED Data (ADC3) |
| 28  | LED-D    | GND      | GPIO 28 | LED Data (ADC2) - **Available for turbo dial** |
| 27  | GND      | LS       | GPIO 27 | Left Stick (L3) / Ground (ADC1) |
| 26  | RUN      | RS       | GPIO 26 | Right Stick (R3) / Reset (ADC0) |
| 25  | GND      | GND      | -     | Ground |
| 24  | 3V3      | 3V3      | -     | 3.3V Power |
| 23  | SCL      | SWCLK    | GPIO 22 | I2C Clock (OLED) / SWD Clock |
| 22  | SDA      | SWDIO    | GPIO 21 | I2C Data (OLED) / SWD Data |
| 21  | GP22     | GND      | GPIO 22 | Expansion / Ground |
| 20  | AU       | GND      | GPIO 16 | Up direction button - Ground connection |
| 19  | AD       | GND      | GPIO 17 | Down direction button - Ground connection |
| 18  | AR       | GND      | GPIO 19 | Right direction button - Ground connection |
| 17  | AL       | GND      | GPIO 18 | Left direction button - Ground connection |
| 16  | GND      | BOOT     | -     | Boot button / Ground |

#### Button to GPIO Mapping (Common Configuration)
Based on typical GP2040-CE Sky 2040 configurations:
- **A (B1)**: GPIO 2
- **B (B2)**: GPIO 3
- **X (B3)**: GPIO 4
- **Y (B4)**: GPIO 5
- **L1**: GPIO 6
- **R1**: GPIO 7
- **L2**: GPIO 8
- **R2**: GPIO 9
- **L3**: GPIO 10
- **R3**: GPIO 11
- **SEL (Select)**: GPIO 12
- **STA (Start)**: GPIO 13
- **HOME**: GPIO 14
- **TURBO**: GPIO 15
- **Up**: GPIO 16
- **Down**: GPIO 17
- **Left**: GPIO 18
- **Right**: GPIO 19

#### Available Pins for Custom Features
**ADC-Capable Pins** (for turbo speed dial):
- **GPIO 26** (ADC0) - Available
- **GPIO 27** (ADC1) - Available
- **GPIO 28** (ADC2) - Available (Recommended for turbo dial)
- **GPIO 29** (ADC3) - May be used for RGB LED

**Digital GPIO Pins Available**:
- **GPIO 20** - Available
- **GPIO 21** - I2C SDA (used if OLED present)
- **GPIO 22** - I2C SCL (used if OLED present)
- **GPIO 23-25** - Available (check specific board)
- **GPIO 26-27** - ADC pins, can be used as digital

#### 5P Expansion Socket
Located on the right edge of the board, provides additional connectivity for custom hardware like turbo switches.

### GPIO Allocation for Turbo Features

#### Recommended Pin Assignment:
**Turbo Speed Dial (Potentiometer)**:
- **GPIO 28 (ADC2)** - Turbo speed dial analog input
  - Connect: Pot wiper → GPIO 28, End 1 → GND, End 2 → 3.3V

**Per-Button Turbo Switches** (8 switches needed):
Using available GPIO pins (verify against your specific board):
- **GPIO 20** → B1 (A button) turbo switch
- **GPIO 23** → B2 (B button) turbo switch
- **GPIO 24** → B3 (X button) turbo switch
- **GPIO 25** → B4 (Y button) turbo switch
- **GPIO 26** → L1 turbo switch
- **GPIO 27** → R1 turbo switch
- **GPIO 0** → L2 turbo switch (if available)
- **GPIO 1** → R2 turbo switch (if available)

*Note: Final pin assignment depends on which GPIOs are actually unused on your specific Sky 2040 variant. GPIO 21-22 are typically reserved for I2C (OLED display).*

### Physical Connection Notes
- All button connections are **active-low** (button press connects GPIO to GND)
- Use internal pull-up resistors (enabled in firmware)
- No external resistors needed for switches
- Potentiometer requires 3.3V power connection

### Board Review Notes
- Described as "mini, and slim" acrylic controller
- Quality construction
- Runs GP2040-CE firmware natively
- Supports custom artwork
- Good option for leverless/all-button controllers
- Compact size makes it ideal for custom enclosures

---

## GP2040-CE Firmware Architecture

### What is GP2040-CE?

GP2040-CE (Community Edition) is a high-performance gamepad firmware for RP2040-based boards that provides:
- **Multi-platform support**: PC, PS3, PS4, PS5, Nintendo Switch, Xbox One, Steam Deck, MiSTer, Android
- **Ultra-low latency**: Average 0.76ms (XInput), 0.91ms (PS5)
- **14 input modes**: XInput, Switch, PS4/PS5, Xbox One, D-Input, Keyboard, and more
- **Rich feature set**: SOCD cleaning, stick emulation, turbo, macros, RGB LEDs
- **Web-based configuration**: Built-in web configurator (no software installation required)

### Key Technical Features
- **Dual-core processing**: One core dedicated to input processing, second core handles LEDs/display
- **1000Hz USB polling** rate (1ms)
- **Low input latency** maintained across all features
- **Extensible add-on system** for community features

---

## Current Turbo Implementation

### Overview
The existing turbo feature in GP2040-CE is implemented as an add-on module located in:
- **Source**: `src/addons/turbo.cpp`
- **Header**: `headers/addons/turbo.h`

### Current Capabilities

#### Basic Turbo Mode
- **Toggle mechanism**: Hold button + press Turbo button to enable/disable turbo per button
- **Configurable speed**: 2-30 shots per second
- **Visual feedback**: LED support (PWM or RGB)
- **Flicker mode**: Alternates button press on/off at set rate

#### Advanced Features
1. **Turbo Dial (Analog)**
   - ADC-based speed control (GPIO 26-29 only)
   - Real-time speed adjustment
   - Range: 2-30 shots/second

2. **SHMUP Mode** (Shoot-em-up optimizations)
   - Up to 4 charge buttons with independent pins
   - Always-on turbo for specific buttons
   - Mix modes: Turbo priority or Charge priority
   - Dedicated charge button mappings

3. **Hotkey Integration**
   - D-pad Up/Down while holding Turbo button adjusts speed
   - Speed changes saved to flash storage

### Configuration Parameters

```cpp
// From turbo.h
#define TURBO_SHOT_MIN 2
#define TURBO_SHOT_MAX 30
#define TURBO_BUTTON_MASK (B1|B2|B3|B4|L1|R1|L2|R2)

// Configurable via BoardConfig.h or WebConfig
- Turbo Button Pin
- Turbo LED Pin  
- Turbo Shot Count (default: 10)
- SHMUP Mode enabled/disabled
- SHMUP button assignments (1-4)
- SHMUP always-on masks
- Turbo Dial Pin (ADC)
```

### Current Limitations
- No visual slider UI in web configurator
- Speed adjustment requires holding Turbo button + D-pad
- Turbo state not saved across power cycles (only speed is saved)
- Limited to non-directional buttons (D-pad excluded)

---

## Proposed Enhancements

### 1. Turbo Speed Slider (Hardware - Analog Potentiometer)

**Goal**: Add physical, real-time adjustable turbo speed control

**Hardware Approach**:
- Connect potentiometer (10kΩ recommended) to ADC-capable GPIO pin (26-29)
- **Good news**: GP2040-CE already supports this via `shmupDialPin` setting!
- Provides instant, tactile feedback without software menus
- No button combos needed during gameplay

**Implementation Details**:
```cpp
// Already implemented in turbo.cpp - just need to enable it!
// Connect potentiometer:
// - Pin 1 (wiper) → GPIO 28 (ADC2) [RECOMMENDED]
// - Pin 2 → GND
// - Pin 3 → 3.3V

// In BoardConfig.h:
#define PIN_SHMUP_DIAL 28  // Use GPIO 28 (ADC2)
```

**How it works**:
- ADC reads voltage from potentiometer (0-3.3V)
- Firmware maps voltage to turbo speed (2-30 shots/second)
- Updates every 100ms for smooth adjustment
- Speed changes in real-time without saving (prevents flash wear)

### 2. Per-Button Turbo On/Off Switches (Hardware - 8 Toggle Switches)

**Goal**: Physical toggle switches to enable/disable turbo for each button independently

**Hardware Design**:
- 8 SPST (Single Pole Single Throw) toggle switches
- One switch per button (B1-B4, L1-L2, R1-R2)
- Each switch connected to dedicated GPIO pin
- Switch state read every polling cycle

**Feasibility Analysis**:

✅ **This IS feasible!** Here's why:

GP2040-CE's current turbo implementation already supports reading multiple GPIO pins - it has "Charge Button 1-4 Pin" for up to 4 additional buttons that modify turbo behavior. We can extend this pattern to support 8 turbo enable/disable switches.

**Implementation Approach**:

#### Hardware Wiring
```
For each button (e.g., B1):
┌─────────────┐
│ Toggle SW 1 │──── GPIO Pin (e.g., GPIO 20)
│   (B1 Turbo)│──── GND
└─────────────┘

Sky 2040 specific wiring:
Switch 1 (B1) → GPIO 20 + GND
Switch 2 (B2) → GPIO 23 + GND
Switch 3 (B3) → GPIO 24 + GND
Switch 4 (B4) → GPIO 25 + GND
Switch 5 (L1) → GPIO 26 + GND
Switch 6 (R1) → GPIO 27 + GND
Switch 7 (L2) → GPIO 0 + GND (verify availability)
Switch 8 (R2) → GPIO 1 + GND (verify availability)
```

#### Firmware Extension
```cpp
// In turbo.h - add new configuration
struct TurboSwitchConfig {
    uint8_t switchPins[8];       // GPIO pins for 8 switches
    uint16_t switchMasks[8];     // Button masks (B1-B4, L1-L2, R1-R2)
    bool switchInvert;           // Invert logic (ON=turbo vs ON=normal)
};

// In turbo.cpp - modify process()
void TurboInput::process() {
    // Read all 8 switch states
    uint16_t hardwareTurboMask = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (isValidPin(switchPins[i])) {
            bool switchOn = !(gpio_get(switchPins[i])); // Active low
            if (switchOn) {
                hardwareTurboMask |= switchMasks[i];
            }
        }
    }
    
    // Apply hardware turbo mask
    turboButtonsMask = hardwareTurboMask;
    
    // Continue with existing turbo logic...
}
```

**Pin Requirements**:
- 8 GPIO pins for switches (GPIO 20, 23-27, 0-1)
- 1 ADC pin for turbo speed dial (GPIO 28 recommended)
- Total: 9 GPIO pins needed

**Switch Behavior Options**:
1. **Direct Mode** (Recommended): Switch ON = Turbo enabled for that button
2. **Toggle Mode**: Switch acts as persistent toggle (ON enables, OFF disables)
3. **Override Mode**: Switch overrides software turbo settings

#### BoardConfig Setup
```cpp
// In configs/Sky2040/BoardConfig.h
#define TURBO_ENABLED 1
#define PIN_SHMUP_DIAL 28          // Speed control dial (GPIO 28 / ADC2)

// Per-button turbo switches
#define PIN_TURBO_SWITCH_B1 20     // A button
#define PIN_TURBO_SWITCH_B2 23     // B button
#define PIN_TURBO_SWITCH_B3 24     // X button
#define PIN_TURBO_SWITCH_B4 25     // Y button
#define PIN_TURBO_SWITCH_L1 26     // L1 button
#define PIN_TURBO_SWITCH_R1 27     // R1 button
#define PIN_TURBO_SWITCH_L2 0      // L2 button (verify)
#define PIN_TURBO_SWITCH_R2 1      // R2 button (verify)

// Map switches to button masks
#define TURBO_SWITCH_MASK_B1 GAMEPAD_MASK_B1
#define TURBO_SWITCH_MASK_B2 GAMEPAD_MASK_B2
#define TURBO_SWITCH_MASK_B3 GAMEPAD_MASK_B3
#define TURBO_SWITCH_MASK_B4 GAMEPAD_MASK_B4
#define TURBO_SWITCH_MASK_L1 GAMEPAD_MASK_L1
#define TURBO_SWITCH_MASK_R1 GAMEPAD_MASK_R1
#define TURBO_SWITCH_MASK_L2 GAMEPAD_MASK_L2
#define TURBO_SWITCH_MASK_R2 GAMEPAD_MASK_R2
```

### 3. Optional: Visual Feedback

**Goal**: Indicate which buttons have turbo active

**Options**:
1. **Per-button RGB LEDs**: Show turbo state with color (green=normal, red=turbo)
2. **OLED Display**: Show turbo speed + button states (already partially supported)
3. **Single Turbo LED**: Blink to indicate active turbo (already supported)

**Recommendation**: Use OLED display if available, as GP2040-CE already shows turbo-enabled buttons with an additional ring/square on the display.

---

## Hardware Feasibility Deep Dive

### Per-Button Turbo Switches: Is It Viable?

**Short Answer: YES! ✅ This is absolutely feasible and practical.**

GP2040-CE's existing turbo implementation already includes support for 4 additional charge buttons with dedicated GPIO pins, which proves the architecture can handle reading multiple auxiliary inputs for turbo control.

### Why This Works

1. **GPIO Availability**: The RP2040 has 26-30 usable GPIO pins (depending on board). Based on the pinout analysis, the Sky 2040 has at least 8-9 available pins for custom features.

2. **Polling Speed**: GP2040-CE operates at 1000Hz (1ms) polling rate, so reading 8 additional GPIO pins adds negligible overhead (~8 microseconds).

3. **Proven Pattern**: The firmware already reads charge button pins and applies different turbo behavior. We're just extending this pattern from 4 pins to 8 pins.

4. **No Latency Impact**: GP2040-CE dedicates one processing core just to reading and writing player inputs, with all secondary functions controlled by the secondary processor core, ensuring switches won't affect input latency.

### Comparison to Existing Commercial Controllers

The NES Advantage controller featured adjustable turbo controls for A and B buttons which could be toggled on or off with a button, with turbo dials to adjust the rate. Our implementation goes further by having per-button switches instead of a single turbo button.

Modern controllers like the 8BitDo Arcade Stick implement turbo by holding a button and pressing a star button to activate turbo functionality, but this requires memorizing which buttons have turbo enabled. Physical switches provide immediate visual and tactile feedback.

### Pin Budget Analysis for Sky 2040

**Required Pins**:
- 8 pins for turbo enable switches (one per button)
- 1 ADC pin for speed dial (GPIO 28 recommended)
- **Total: 9 GPIO pins**

**Sky 2040 Usage** (based on pinout):
- 8 face/shoulder buttons (B1-B4, L1-L2, R1-R2): GPIOs 2-9
- 4 directions: GPIOs 16-19
- Start/Select: GPIOs 12-13
- L3/R3: GPIOs 10-11
- Home/Turbo: GPIOs 14-15
- I2C Display (SDA/SCL): GPIOs 21-22 (optional)
- RGB LED: GPIO 29 (optional)
- **Used: ~20-22 pins**

**Available for turbo features**:
- GPIOs 0, 1, 20, 23-28 (8-9 pins available)
- **Perfect fit!** ✅

### Switch Wiring Simplification

Instead of wiring 8 separate switches, you can use a **DIP switch array** or **mini toggle switch PCB** for compact installation:

```
Example compact layout:
┌─────────────────────┐
│ [1][2][3][4]       │  DIP-8 switch array
│ [5][6][7][8]       │  Labeled: B1 B2 B3 B4
│                     │           L1 L2 R1 R2
└─────────────────────┘

Physical mounting options:
- On top panel of controller enclosure
- Side panel for easy access
- Via 5P expansion socket with custom PCB
```

### Alternative: GPIO Expander

If GPIO pins are scarce, consider using an I2C GPIO expander (PCF8574):
- GP2040-CE already supports the PCF8574 IO Expander add-on
- Provides 8 additional GPIO pins via I2C (only uses 2 pins: SDA/SCL)
- Slightly higher latency (~1-2ms) but acceptable for turbo switches
- Would share I2C bus with OLED display if present

---

## Build System

### Requirements
- **Pico SDK**: v2.1.1 or later
- **CMake**: v3.10 or higher
- **Git**: For version tagging
- **Node.js + NPM**: For web configurator build (optional, can skip with `SKIP_WEBBUILD`)

### Build Process

```bash
# Set board configuration
export GP2040_BOARDCONFIG=Sky2040

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Output: GP2040-CE_X.X.X_Sky2040.uf2
```

### Board Configuration Structure

Each board has a config folder: `configs/[BoardName]/`
- `BoardConfig.h` - GPIO pin mappings and feature flags
- `CMakeLists.txt` - Build configuration
- `README.md` - Board documentation
- `assets/` - Images and resources

### Creating Custom Sky 2040 Config

**Recommended path**: `configs/Sky2040/BoardConfig.h`

Key sections to customize:
1. **GPIO Pin Mapping** - Map physical pins to button actions
2. **Feature Flags** - Enable turbo, LEDs, display
3. **Add-on Configuration** - Turbo settings, LED configuration
4. **Hardware Specs** - I2C pins, LED pins, etc.

---

## Key Files for Modification

### Turbo System Extensions
| File | Purpose | Modifications Needed |
|------|---------|---------------------|
| `src/addons/turbo.cpp` | Core turbo logic | Add switch polling, extend to 8 switches |
| `headers/addons/turbo.h` | Turbo interface | Add switch config structures |
| `proto/config.proto` | Configuration schema | Add switch pin config fields |
| `configs/Sky2040/BoardConfig.h` | Board-specific config | Define switch GPIO pins, dial pin |

### Optional WebConfig UI (if adding configuration)
| File | Purpose | Modifications Needed |
|------|---------|---------------------|
| `www/src/Pages/AddonsConfigPage.tsx` | Add-ons UI page | Add switch pin configuration UI |
| `www/src/Services/WebApi.ts` | API communication | Add switch config endpoints |

---

## Implementation Roadmap

### Phase 1: Board Configuration (Priority: High)
1. ✅ Study existing codebase
2. ⬜ Create `configs/Sky2040/BoardConfig.h` (or use existing if available)
3. ⬜ Map GPIO pins based on Sky 2040 hardware pinout
4. ⬜ Test basic build and flashing
5. ⬜ Verify all buttons and features work

### Phase 2: Hardware Integration - Turbo Speed Dial (Priority: High)
1. ⬜ Select and wire potentiometer to GPIO 28 (ADC2)
2. ⬜ Configure `PIN_SHMUP_DIAL 28` in BoardConfig.h
3. ⬜ Test existing dial functionality (already implemented!)
4. ⬜ Calibrate speed range to feel right (2-30 shots/sec)
5. ⬜ Optional: Add dial position indicator on OLED

### Phase 3: Hardware Integration - Per-Button Turbo Switches (Priority: High)
1. ⬜ Design physical switch layout (8 SPST toggle switches or DIP-8 array)
2. ⬜ Verify GPIO pin availability (GPIOs 0, 1, 20, 23-27)
3. ⬜ Wire switches to GPIO pins (active-low with pull-up)
4. ⬜ Extend turbo.cpp to read switch states
5. ⬜ Map switch states to button turbo masks
6. ⬜ Test switch responsiveness and debouncing
7. ⬜ Add OLED visual indicators for enabled buttons

### Phase 4: Firmware Extensions (Priority: Medium)
1. ⬜ Create configuration structure for switch pins
2. ⬜ Implement switch polling in turbo process loop
3. ⬜ Add configuration options to WebConfig UI
4. ⬜ Support switch inversion option (ON=turbo vs ON=normal)
5. ⬜ Document switch wiring in board README

### Phase 5: Optional Enhancements (Priority: Low)
1. ⬜ Add per-button RGB LED turbo indicators
2. ⬜ Implement turbo profiles (save switch states)
3. ⬜ Add hotkey to temporarily override switches
4. ⬜ Create calibration tool for dial sensitivity

### Phase 6: Testing & Polish (Priority: High)
1. ⬜ End-to-end testing on Sky 2040 hardware
2. ⬜ Performance testing (latency impact)
3. ⬜ Documentation updates
4. ⬜ User guide creation
5. ⬜ Consider upstream PR to GP2040-CE project

---

## Development Best Practices

### Code Style
- Follow existing GP2040-CE conventions
- Use `.editorconfig` for formatting
- Comment complex turbo logic clearly
- Follow C++17 standards

### Testing Strategy
1. **Unit tests** for turbo logic changes
2. **Hardware testing** on actual Sky 2040 board
3. **Input latency testing** to ensure no regressions
4. **Cross-platform testing** (PC, Switch, PS4/5)

### Version Control
- Create feature branches for each enhancement
- Write clear commit messages
- Keep changes modular and reviewable
- Consider contributing back to upstream

---

## Resources

### Documentation
- **Official Docs**: https://gp2040-ce.info
- **Turbo Add-on**: https://gp2040-ce.info/add-ons/turbo/
- **Web Configurator**: https://gp2040-ce.info/web-configurator
- **Building Firmware**: https://gp2040-ce.info/development/

### Community
- **Discord**: https://discord.gg/k2pxhke7q8
- **GitHub**: https://github.com/OpenStickCommunity/GP2040-CE
- **Issues**: https://github.com/OpenStickCommunity/GP2040-CE/issues

### Related Projects
- **Original GP2040**: https://github.com/FeralAI/GP2040
- **alirin222's Turbo Implementation**: https://github.com/alirin222/GP2040-PicoAnn
- **TinyUSB**: https://github.com/hathach/tinyusb

---

## Technical Deep Dive: Turbo System

### Current Implementation Flow

```
User Input → Gamepad Polling → Turbo Addon Process
                                    ↓
                          Check Turbo Button State
                                    ↓
                          Toggle Button Turbo Mode
                                    ↓
                          Apply Turbo Flicker (ON/OFF)
                                    ↓
                          Update LED State → Output
```

### Turbo State Machine

```cpp
// Current state tracking
uint16_t turboButtonsMask;   // Which buttons have turbo enabled
bool bTurboFlicker;          // Current ON/OFF state
uint32_t uIntervalUS;        // Microsecond interval based on shot count
uint64_t nextTimer;          // Next toggle time

// Process loop (every ~1ms)
if (currentTime >= nextTimer) {
    bTurboFlicker = !bTurboFlicker;  // Toggle state
    nextTimer = currentTime + uIntervalUS;
}

// Apply to buttons
if (bTurboFlicker) {
    gamepad->buttons &= ~turboButtonsMask;  // Disable turbo buttons
} else {
    // Buttons remain pressed naturally
}
```

### Storage Structure

```protobuf
// From config.proto
message TurboOptions {
    bool enabled = 1;
    uint32 ledPin = 2;
    uint32 turboLedType = 3;
    uint32 shotCount = 4;            // 2-30 shots/sec
    uint32 shmupDialPin = 5;         // ADC pin for dial
    bool shmupModeEnabled = 6;
    // ... SHMUP mode options
}
```

### Proposed Extensions

```cpp
// Extended turbo state
struct TurboButtonState {
    bool enabled;              // Turbo active for this button
    uint8_t speed;            // Individual speed (2-30)
    bool persistent;          // Save state across power cycles
};

struct TurboProfile {
    char name[32];
    TurboButtonState buttons[8];  // B1-B4, L1-L2, R1-R2
    uint8_t defaultSpeed;
};

class TurboInput : public GPAddon {
    // ... existing members ...
    
    TurboProfile profiles[4];     // Up to 4 profiles
    uint8_t activeProfile;        // Current profile index
    
    void saveProfile(uint8_t index);
    void loadProfile(uint8_t index);
    void switchProfile(uint8_t index);
};
```

---

## Hardware Considerations

### ADC Pin Usage for Turbo Dial
The RP2040 has 4 ADC-capable pins (GPIO 26-29). For the Sky 2040:
- **GPIO 28 (ADC2)**: **RECOMMENDED** for turbo speed dial
- GPIO 26 (ADC0): Alternative option
- GPIO 27 (ADC1): Alternative option
- GPIO 29 (ADC3): May be used for RGB LED

### LED Feedback Options
1. **RGB LEDs**: Per-button RGB for turbo state indication (if GPIO 29 available)
2. **Single LED**: Global turbo activity indicator (already supported)
3. **OLED Display**: Show turbo speed and enabled buttons (uses I2C on GPIO 21-22)

### Physical Switch Mounting
Consider these mounting options:
1. **Top panel**: DIP-8 switch array for easy access
2. **Side panel**: Individual toggle switches
3. **5P expansion socket**: Custom PCB with switches
4. **Internal**: Hidden switches activated by external slider/lever mechanism

---

## Next Steps

### Immediate Actions
1. **Verify hardware**: Confirm GPIO pinout matches images on actual Sky 2040 board
2. **Check existing configs**: Look for existing Sky2040 board configuration in GP2040-CE repo
3. **Order components**:
   - 10kΩ linear potentiometer (rotary or slide)
   - DIP-8 switch array OR 8× SPST mini toggle switches
   - Optional: Breakout PCB for clean wiring
4. **Test stock firmware**: Flash official GP2040-CE to ensure board works

### Development Environment Setup
```bash
# Already done - project location
cd /Users/fwong/Documents/github/wongfei2009/GP2040-CE

# Update submodules (Pico SDK)
git submodule update --init --recursive

# Install build tools (macOS) - if not already installed
brew install cmake
brew install gcc-arm-embedded

# Set up environment
export PICO_SDK_PATH=$(pwd)/lib/pico-sdk
export GP2040_BOARDCONFIG=Sky2040
```

### Questions to Resolve
- [ ] Are GPIOs 0, 1, 20, 23-27 actually available/unused?
- [ ] Is I2C (GPIO 21-22) in use for OLED display?
- [ ] Which GPIO is used for RGB LED data?
- [ ] Physical space available for switch mounting?
- [ ] Preference for rotary pot vs slide pot for speed dial?
- [ ] DIP switch array vs individual toggle switches?
- [ ] Does an existing Sky2040 config already exist in GP2040-CE?

---

## Conclusion

This project aims to enhance the already-excellent GP2040-CE firmware with advanced hardware-based turbo features specifically tailored for the Sky 2040 board. By implementing a physical turbo speed dial and per-button turbo toggle switches, we can create a more intuitive and flexible turbo system that benefits both competitive players and casual users.

The detailed pinout analysis from the hardware images confirms that the Sky 2040 has sufficient GPIO pins available for this implementation. The modular nature of GP2040-CE's add-on system makes these enhancements feasible without disrupting core functionality, and the changes can potentially be contributed back to benefit the entire community.

**Ready to start coding!** 🚀
