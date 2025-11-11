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
- **Size**: 5.1cm × 3.1cm
- **Mounting holes**: 45mm × 25mm centers, 3mm diameter
- **Firmware**: GP2040-CE (open source)
- **Features**: 
  - USB-C connectivity
  - Exposed PCB for easy modification
  - LED support for button lighting
  - Compact, acrylic construction

### Pin Layout (from image)
The board has dual rows of GPIO pins with the following labeled sections:
- **Top Row**: Boot button, left/up/down/right directional inputs, OLED interface, RGB interface
- **Bottom Row**: USB connection, function button outputs, HOME/L3/R3/system buttons
- **5P Expansion Socket**: Located on right edge

### Board Review Notes
- Described as "mini, and slim" acrylic controller
- Quality construction
- Runs GP2040-CE firmware natively
- Supports custom artwork
- Good option for leverless/all-button controllers

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
// - Pin 1 (wiper) → GPIO 26/27/28/29 (ADC pin)
// - Pin 2 → GND
// - Pin 3 → 3.3V

// In BoardConfig.h:
#define PIN_SHMUP_DIAL 28  // Or 26, 27, 29
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
│ Toggle SW 1 │──── GPIO Pin (e.g., GPIO 22)
│   (B1 Turbo)│──── GND
└─────────────┘

Repeat for all 8 buttons with unique GPIO pins
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
- 8 GPIO pins for switches (can use any available digital pins)
- 1 ADC pin for turbo speed dial (GPIO 26-29)
- Total: 9 GPIO pins needed

**Switch Behavior Options**:
1. **Direct Mode** (Recommended): Switch ON = Turbo enabled for that button
2. **Toggle Mode**: Switch acts as persistent toggle (ON enables, OFF disables)
3. **Override Mode**: Switch overrides software turbo settings

#### BoardConfig Setup
```cpp
// In configs/Sky2040/BoardConfig.h
#define TURBO_ENABLED 1
#define PIN_SHMUP_DIAL 28          // Speed control dial

// Per-button turbo switches
#define PIN_TURBO_SWITCH_B1 22     // GPIO assignments
#define PIN_TURBO_SWITCH_B2 23
#define PIN_TURBO_SWITCH_B3 24
#define PIN_TURBO_SWITCH_B4 25
#define PIN_TURBO_SWITCH_L1 26
#define PIN_TURBO_SWITCH_R1 27
#define PIN_TURBO_SWITCH_L2 29
#define PIN_TURBO_SWITCH_R2 21

// Map switches to button masks
#define TURBO_SWITCH_MASK_B1 GAMEPAD_MASK_B1
#define TURBO_SWITCH_MASK_B2 GAMEPAD_MASK_B2
// ... etc
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

1. **GPIO Availability**: The RP2040 has 26-30 usable GPIO pins (depending on board). The Sky 2040 likely has several unused pins that can be allocated to switches.

2. **Polling Speed**: GP2040-CE operates at 1000Hz (1ms) polling rate, so reading 8 additional GPIO pins adds negligible overhead (~8 microseconds).

3. **Proven Pattern**: The firmware already reads charge button pins and applies different turbo behavior. We're just extending this pattern from 4 pins to 8 pins.

4. **No Latency Impact**: GP2040-CE dedicates one processing core just to reading and writing player inputs, with all secondary functions controlled by the secondary processor core, ensuring switches won't affect input latency.

### Comparison to Existing Commercial Controllers

The NES Advantage controller featured adjustable turbo controls for A and B buttons which could be toggled on or off with a button, with turbo dials to adjust the rate. Our implementation goes further by having per-button switches instead of a single turbo button.

Modern controllers like the 8BitDo Arcade Stick implement turbo by holding a button and pressing a star button to activate turbo functionality, but this requires memorizing which buttons have turbo enabled. Physical switches provide immediate visual and tactile feedback.

### Pin Budget Analysis

**Required Pins**:
- 8 pins for turbo enable switches (one per button)
- 1 ADC pin for speed dial (GPIO 26-29)
- **Total: 9 GPIO pins**

**Typical Sky 2040 Usage** (estimated):
- 4 face buttons (B1-B4): 4 pins
- 4 shoulder buttons (L1-L2, R1-R2): 4 pins  
- 4 directions: 4 pins
- Start/Select (S1-S2): 2 pins
- L3/R3: 2 pins (if used)
- Home/Capture: 2 pins (if used)
- RGB LED data: 1 pin
- I2C Display (SDA/SCL): 2 pins
- **Subtotal: ~21 pins**

**Remaining**: ~8-9 pins available (depending on exact board configuration)

✅ **This fits perfectly within the pin budget!**

### Switch Wiring Simplification

Instead of wiring 8 separate switches, you can use a **DIP switch array** or **mini toggle switch PCB** for compact installation:

```
Example compact layout:
┌─────────────────────┐
│ [1][2][3][4]       │  DIP-8 switch array
│ [5][6][7][8]       │  Labeled: B1 B2 B3 B4
│                     │           L1 L2 R1 R2
└─────────────────────┘
```

### Alternative: GPIO Expander

If GPIO pins are scarce, consider using an I2C GPIO expander (PCF8574):
- GP2040-CE already supports the PCF8574 IO Expander add-on
- Provides 8 additional GPIO pins via I2C (only uses 2 pins: SDA/SCL)
- Slightly higher latency (~1-2ms) but acceptable for turbo switches

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
2. ⬜ Create `configs/Sky2040/BoardConfig.h`
3. ⬜ Map GPIO pins based on Sky 2040 hardware
4. ⬜ Test basic build and flashing
5. ⬜ Verify all buttons and features work

### Phase 2: Hardware Integration - Turbo Speed Dial (Priority: High)
1. ⬜ Select and wire potentiometer to ADC pin (GPIO 26-29)
2. ⬜ Configure `PIN_SHMUP_DIAL` in BoardConfig.h
3. ⬜ Test existing dial functionality (already implemented!)
4. ⬜ Calibrate speed range to feel right (2-30 shots/sec)
5. ⬜ Optional: Add dial position indicator on OLED

### Phase 3: Hardware Integration - Per-Button Turbo Switches (Priority: High)
1. ⬜ Design physical switch layout (8 SPST toggle switches)
2. ⬜ Identify 8 available GPIO pins on Sky 2040
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

### ADC Pin Usage for Slider
The RP2040 has 4 ADC-capable pins (GPIO 26-29). The Sky 2040 may have some already assigned:
- Check existing schematic/documentation
- Consider using GPIO 28 (currently shown as turbo in Pico config)
- May need hardware modification to add physical potentiometer

### LED Feedback Options
1. **RGB LEDs**: Per-button RGB for turbo state indication
2. **Single LED**: Global turbo activity indicator (already supported)
3. **OLED Display**: Show turbo speed and enabled buttons

### Button Matrix
- Ensure GPIO pins chosen don't conflict with button matrix
- Consider using available pins in the 5P expansion socket

---

## Next Steps

### Immediate Actions
1. **Verify hardware**: Map exact GPIO connections on Sky 2040
2. **Test stock firmware**: Flash official GP2040-CE to ensure board works
3. **Create board config**: Set up `configs/Sky2040/` directory
4. **Build and test**: Compile custom config and verify basic functionality

### Development Environment Setup
```bash
# Clone your fork (already done)
cd /Users/fwong/Documents/github/wongfei2009/GP2040-CE

# Install Pico SDK (if not already)
git submodule update --init --recursive

# Install build tools (macOS)
brew install cmake
brew install gcc-arm-embedded

# Set up environment
export PICO_SDK_PATH=/path/to/pico-sdk
export GP2040_BOARDCONFIG=Sky2040
```

### Questions to Answer
1. What GPIO pins are available/unused on Sky 2040?
2. Does the board have ADC-accessible pins exposed?
3. What LED capabilities exist (individual RGB or single indicator)?
4. Are there any existing hardware turbo implementations to reference?
5. What is the target user experience for turbo control?

---

## Conclusion

This project aims to enhance the already-excellent GP2040-CE firmware with advanced hardware-based turbo features specifically tailored for the Sky 2040 board. By implementing a physical turbo speed dial and per-button turbo toggle switches, we can create a more intuitive and flexible turbo system that benefits both competitive players and casual users.

The modular nature of GP2040-CE's add-on system makes these enhancements feasible without disrupting core functionality, and the changes can potentially be contributed back to benefit the entire community.

**Ready to start coding!** 🚀
