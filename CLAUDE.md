# Sky 2040 GP2040-CE Turbo Enhancement Project

## Quick Summary
**Goal**: Add hardware-based turbo controls to Sky 2040 joystick
- ✅ Analog turbo speed dial (potentiometer, 2-30 shots/sec)
- ✅ 8 per-button turbo switches (B1-B4, L1-L2, R1-R2)
- **Strategy**: Use I2C GPIO expander (MCP23017) for switches
- **GPIO Required**: 1 ADC (speed dial) + I2C bus (shared with display)
- **Hardware**: MCP23017 chip + potentiometer + 8 switches

## Project Location
- **Fork**: `/Users/fwong/Documents/github/wongfei2009/GP2040-CE`
- **Upstream**: OpenStickCommunity/GP2040-CE

## Manufacturer Pinout Data

### Connector Pins 1-15
| Pin | Outer | Inner | Label | Description |
|---|---|---|---|---|
| 1 | GND | A | A | Button A (Cross) |
| 2 | GND | B | B | Button B (Circle) |
| 3 | GND | X | X | Button X (Square) |
| 4 | GND | Y | Y | Button Y (Triangle) |
| 5 | GND | L1 | L1 | Button L1 |
| 6 | GND | R1 | R1 | Button R1 |
| 7 | GND | L2 | L2 | Button L2 |
| 8 | GND | R2 | R2 | Button R2 |
| 9 | GND | L3 | L3 | Button L3 |
| 10 | GND | R3 | R3 | Button R3 |
| 11 | GND | SEL | Select | 选择键 |
| 12 | GND | STA | Start | 开始键 |
| 13 | GND | HOME | Home | PS键 |
| 14 | GND | TURBO | Turbo | 连发键 |
| 15 | GND | TU-LED| Turbo LED | 连发指示灯 |

### Connector Pins 16-30
| Pin | Inner | Outer | Label | Description |
|---|---|---|---|---|
| 16 | GND | BOOT | BOOT | Bootselect | 
| 17 | AL | GND | AL | D-Pad Left | 
| 18 | AR | GND | AR | D-Pad Right | 
| 19 | AD | GND | AD | D-Pad Down | 
| 20 | AU | GND | AU | D-Pad Up | 
| 21 | GP22 | GND | GP22 | Expansion Key | 
| 22 | SDA | SWDIO | SDA / SWDIO | I2C Data / Debug |
| 23 | SCL | SWCLK | SCL / SWCLK | I2C Clock / Debug |
| 24 | 3V3 | 3V3 | 3.3V | Power | 
| 25 | GND | GND | GND | Ground | 
| 26 | RUN | RS | RS | Spare Key / GPIO 26 (ADC0) | 
| 27 | GND | LS | LS | Spare Key / GPIO 27 (ADC1) | 
| 28 | LED-D | GND | LED-D | RGB LED Data (GPIO 28) | 
| 29 | GND | A2 | A2 | Capture Button (GPIO 21) |
| 30 | 5V | GND | 5V | Power |

---

## Sky 2040 Configuration

### Base Configuration
- **Board Type**: Raspberry Pi Pico form factor
- **Only Difference**: GPIO 2/3 swapped for UP/DOWN vs standard Pico

### GPIO Mappings (RP2040)

#### Buttons (Active-Low with Pull-Up)
| Button | GPIO | Physical Pin | Function | Notes |
|--------|------|--------------|----------|-------|
| B1 (A/Cross) | 6 | Pin 1 (Connector) | Face button | Turbo-capable |
| B2 (B/Circle) | 7 | Pin 2 (Connector) | Face button | Turbo-capable |
| B3 (X/Square) | 10 | Pin 3 (Connector) | Face button | Turbo-capable |
| B4 (Y/Triangle) | 11 | Pin 4 (Connector) | Face button | Turbo-capable |
| L1 | 13 | Pin 5 (Connector) | Shoulder | Turbo-capable |
| R1 | 12 | Pin 6 (Connector) | Shoulder | Turbo-capable |
| L2 | 9 | Pin 7 (Connector) | Trigger | Turbo-capable |
| R2 | 8 | Pin 8 (Connector) | Trigger | Turbo-capable |
| Up | 3 | Pin 20 (Connector) | D-pad | ⚠️ Swapped vs Pico |
| Down | 2 | Pin 19 (Connector) | D-pad | ⚠️ Swapped vs Pico |
| Left | 5 | Pin 17 (Connector) | D-pad | |
| Right | 4 | Pin 18 (Connector) | D-pad | |
| Select (S1) | 16 | Pin 11 (Connector) | System | |
| Start (S2) | 17 | Pin 12 (Connector) | System | |
| L3 | 18 | Pin 9 (Connector) | Stick | |
| R3 | 19 | Pin 10 (Connector) | Stick | |
| Home (A1) | 20 | Pin 13 (Connector) | PS/Guide | |
| Capture (A2) | 21 | Pin 29 (Connector) | Capture | |
| Turbo | 14 | Pin 14 (Connector) | Turbo toggle | Existing button |

#### Peripherals
- **Display (I2C)**: SDA=GPIO0, SCL=GPIO1, Addr=0x3C (Connector Pins 22 & 23)
- **RGB LEDs**: Data=GPIO28 (Connector Pin 28)
- **Turbo Button**: GPIO14 (Connector Pin 14)
- **Turbo Speed Dial**: ADC=GPIO26 (Connector Pin 26, Outer RS)

---

## Hardware Implementation

### Why I2C Expander?
**Problem**: Raspberry Pi Pico form factor only exposes 5 usable GPIOs
- GP23-25, GP29 are not available (used internally or onboard LED)
- We need 8 switches + 1 speed dial = 9 pins total

**Solution**: MCP23017 I2C GPIO Expander
- Provides 16 additional GPIO pins via I2C
- Shares I2C bus with display (no extra pins needed)
- Configurable I2C address (default 0x20)
- Built-in pull-up resistors
- Widely available and inexpensive (~$1-2)

### Component List

#### 1. MCP23017 I2C GPIO Expander
**Specs**:
- 16 GPIO pins (we need 8)
- I2C interface (3.3V compatible)
- Configurable address: 0x20-0x27
- Internal pull-ups available
- DIP-28 or SOIC-28 package

**Pinout**:
```
MCP23017 (DIP-28):
         ┌───∪───┐
    GPB0 │1    28│ GPA7
    GPB1 │2    27│ GPA6
    GPB2 │3    26│ GPA5
    GPB3 │4    25│ GPA4
    GPB4 │5    24│ GPA3
    GPB5 │6    23│ GPA2
    GPB6 │7    22│ GPA1
    GPB7 │8    21│ GPA0
     VDD │9    20│ INTA
     VSS │10   19│ INTB
      NC │11   18│ RESET
     SCL │12   17│ A2
     SDA │13   16│ A1
      NC │14   15│ A0
         └───────┘
```

#### 2. Turbo Speed Dial
- 10kΩ linear potentiometer (rotary or slide)

#### 3. Turbo Switches
- 8× SPST toggle switches OR 1× DIP-8 switch array

#### 4. Wiring Components
- Breadboard (for prototyping)
- Jumper wires
- 0.1μF ceramic capacitor (for MCP23017 power filtering)

### Wiring Diagram

#### MCP23017 Connection to Sky 2040
```
MCP23017 Pin → Sky 2040 Pin
─────────────────────────────
Pin 9 (VDD)     → Pin 36 (3V3 OUT)
Pin 10 (VSS)    → Pin 38 (GND)
Pin 12 (SCL)    → Pin 2 (GP01 / SCL) - Shared with display
Pin 13 (SDA)    → Pin 1 (GP00 / SDA) - Shared with display
Pin 18 (RESET)  → Pin 36 (3V3) via 10kΩ resistor (pull-up)
Pin 15 (A0)     → Pin 38 (GND) - Address = 0x20
Pin 16 (A1)     → Pin 38 (GND) - Address = 0x20
Pin 17 (A2)     → Pin 38 (GND) - Address = 0x20

Power filtering:
- 0.1μF capacitor between VDD (Pin 9) and VSS (Pin 10)
```

**I2C Address Configuration**:
```
A2  A1  A0  | I2C Address
─────────────────────────
GND GND GND | 0x20 (default)
GND GND VDD | 0x21
GND VDD GND | 0x22
GND VDD VDD | 0x23
VDD GND GND | 0x24
VDD GND VDD | 0x25
VDD VDD GND | 0x26
VDD VDD VDD | 0x27
```
**Use 0x20** (all address pins to GND) to avoid conflict with display (0x3C)

#### Turbo Switches Connection to MCP23017
**8 switches → MCP23017 GPA pins (active-low with pull-up)**

```
Switch Wiring (each switch):
┌─────────────────┐
│  SPST Switch    │
│  Terminal 1  ───┼──→ MCP23017 GPA pin (see table below)
│  Terminal 2  ───┼──→ GND
└─────────────────┘

Pin Assignment:
Button  → MCP23017 Pin → GPA Pin
─────────────────────────────────
B1 (A)  → Pin 21       → GPA0
B2 (B)  → Pin 22       → GPA1
B3 (X)  → Pin 23       → GPA2
B4 (Y)  → Pin 24       → GPA3
L1      → Pin 25       → GPA4
R1      → Pin 26       → GPA5
L2      → Pin 27       → GPA6
R2      → Pin 28       → GPA7
```

**Logic**:
- Switch CLOSED (ON) → GPA pin reads LOW → Turbo ENABLED
- Switch OPEN (OFF) → GPA pin reads HIGH → Normal operation

#### Turbo Speed Dial Connection
```
10kΩ Potentiometer:
┌──────────────────┐
│  Potentiometer   │
│  Pin 1 (CCW)  ───┼──→ GND (Connector Pin 25)
│  Pin 2 (Wiper) ──┼──→ GPIO 26 (Connector Pin 26, Outer RS)
│  Pin 3 (CW)  ────┼──→ 3.3V (Connector Pin 24)
└──────────────────┘

Output Range: 0V (min speed) to 3.3V (max speed)
Turbo Speed: 2-30 shots/sec based on voltage
```

---

## Software Implementation

### Phase 1: Create Sky2040 BoardConfig
**File**: `configs/Sky2040/BoardConfig.h`

**Key Changes from Pico**:
```cpp
// Board identification
#define BOARD_CONFIG_LABEL "Sky2040"

// D-pad GPIO swap (Sky2040-specific)
#define GPIO_PIN_02 GpioAction::BUTTON_PRESS_DOWN  // Swapped
#define GPIO_PIN_03 GpioAction::BUTTON_PRESS_UP    // Swapped

// Keep RGB LEDs enabled (no conflict with Option C)
#define BOARD_LEDS_PIN 28  // Keep enabled ✅

// Turbo speed dial (ADC0)
#define PIN_SHMUP_DIAL 26

// I2C Turbo switches configuration
#define TURBO_I2C_SWITCHES_ENABLED 1
#define TURBO_I2C_SDA_PIN 0     // Shared with display
#define TURBO_I2C_SCL_PIN 1     // Shared with display
#define TURBO_I2C_BLOCK i2c0    // Same I2C block as display
#define TURBO_I2C_SPEED 400000  // 400kHz (same as display)
#define TURBO_I2C_ADDR 0x20     // MCP23017 address
```

### Phase 2: Add MCP23017 Driver
**Files**: `src/addons/turbo.cpp`, `headers/addons/turbo.h`, `lib/mcp23017/mcp23017.h`

#### Create MCP23017 Library
**File**: `lib/mcp23017/mcp23017.h`
```cpp
#ifndef MCP23017_H
#define MCP23017_H

#include "hardware/i2c.h"
#include <stdint.h>

// MCP23017 Register Addresses
#define MCP23017_IODIRA   0x00  // I/O direction A
#define MCP23017_IODIRB   0x01  // I/O direction B
#define MCP23017_GPPUA    0x0C  // Pull-up A
#define MCP23017_GPPUB    0x0D  // Pull-up B
#define MCP23017_GPIOA    0x12  // Port A
#define MCP23017_GPIOB    0x13  // Port B

class MCP23017 {
public:
    MCP23017(i2c_inst_t* i2c, uint8_t addr);
    
    bool init();                    // Initialize chip
    void setPinMode(uint8_t pin, bool input);
    void setPullup(uint8_t pin, bool enable);
    bool readPin(uint8_t pin);
    uint16_t readAll();             // Read all 16 pins
    
private:
    i2c_inst_t* i2c_;
    uint8_t addr_;
    
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};

#endif // MCP23017_H
```

**File**: `lib/mcp23017/mcp23017.cpp`
```cpp
#include "mcp23017.h"

MCP23017::MCP23017(i2c_inst_t* i2c, uint8_t addr) 
    : i2c_(i2c), addr_(addr) {}

bool MCP23017::init() {
    // Configure all pins on Port A as inputs with pull-ups
    writeRegister(MCP23017_IODIRA, 0xFF);  // All inputs
    writeRegister(MCP23017_GPPUA, 0xFF);   // All pull-ups enabled
    
    // Port B unused (all inputs, no pull-ups)
    writeRegister(MCP23017_IODIRB, 0xFF);
    writeRegister(MCP23017_GPPUB, 0x00);
    
    return true;
}

void MCP23017::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(i2c_, addr_, buf, 2, false);
}

uint8_t MCP23017::readRegister(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c_, addr_, &reg, 1, true);
    i2c_read_blocking(i2c_, addr_, &value, 1, false);
    return value;
}

uint16_t MCP23017::readAll() {
    uint8_t portA = readRegister(MCP23017_GPIOA);
    uint8_t portB = readRegister(MCP23017_GPIOB);
    return (portB << 8) | portA;
}

bool MCP23017::readPin(uint8_t pin) {
    if (pin < 8) {
        uint8_t portA = readRegister(MCP23017_GPIOA);
        return (portA & (1 << pin)) != 0;
    } else {
        uint8_t portB = readRegister(MCP23017_GPIOB);
        return (portB & (1 << (pin - 8))) != 0;
    }
}
```

#### Modify Turbo Add-on for I2C Switches
**File**: `headers/addons/turbo.h`
```cpp
// Add to existing TurboInput class

#ifdef TURBO_I2C_SWITCHES_ENABLED
#include "mcp23017.h"
#endif

class TurboInput : public GPAddon {
public:
    // ... existing members ...
    
#ifdef TURBO_I2C_SWITCHES_ENABLED
    MCP23017* mcp_;  // I2C expander instance
#endif
};
```

**File**: `src/addons/turbo.cpp`
```cpp
#include "addons/turbo.h"

void TurboInput::setup() {
    // ... existing setup code ...
    
    #ifdef TURBO_I2C_SWITCHES_ENABLED
    // Initialize MCP23017 on shared I2C bus
    // Note: I2C already initialized by display addon
    mcp_ = new MCP23017(TURBO_I2C_BLOCK, TURBO_I2C_ADDR);
    
    if (!mcp_->init()) {
        // Handle initialization error
        // Could set flag to disable I2C switches
        delete mcp_;
        mcp_ = nullptr;
    }
    #endif
}

void TurboInput::process() {
    #ifdef TURBO_I2C_SWITCHES_ENABLED
    if (mcp_) {
        // Read all 8 switches from MCP23017 Port A (active-low)
        uint8_t switchStates = mcp_->readRegister(MCP23017_GPIOA);
        
        // Build turbo mask from switch states
        uint16_t hardwareTurboMask = 0;
        
        // Map MCP23017 pins to button masks (active-low)
        if (!(switchStates & 0x01)) hardwareTurboMask |= GAMEPAD_MASK_B1;  // GPA0
        if (!(switchStates & 0x02)) hardwareTurboMask |= GAMEPAD_MASK_B2;  // GPA1
        if (!(switchStates & 0x04)) hardwareTurboMask |= GAMEPAD_MASK_B3;  // GPA2
        if (!(switchStates & 0x08)) hardwareTurboMask |= GAMEPAD_MASK_B4;  // GPA3
        if (!(switchStates & 0x10)) hardwareTurboMask |= GAMEPAD_MASK_L1;  // GPA4
        if (!(switchStates & 0x20)) hardwareTurboMask |= GAMEPAD_MASK_R1;  // GPA5
        if (!(switchStates & 0x40)) hardwareTurboMask |= GAMEPAD_MASK_L2;  // GPA6
        if (!(switchStates & 0x80)) hardwareTurboMask |= GAMEPAD_MASK_R2;  // GPA7
        
        // Hardware switches override software turbo settings
        turboButtonsMask = hardwareTurboMask;
    }
    #endif
    
    // Continue with existing turbo flicker logic
    // Speed dial already supported via pinShmupDial (GPIO 26)
    // ... existing turbo processing code ...
}
```

---

## Implementation Roadmap

### Phase 1: BoardConfig Setup ⏱️ 1 hour
**Tasks**:
1. Create `configs/Sky2040/` directory
2. Copy `configs/Pico/BoardConfig.h` → `configs/Sky2040/BoardConfig.h`
3. Apply changes:
   - Swap GPIO 2 ↔ 3
   - Set `BOARD_CONFIG_LABEL "Sky2040"`
   - Keep RGB enabled: `#define BOARD_LEDS_PIN 28`
   - Add I2C turbo switch definitions
   - Enable speed dial: `#define PIN_SHMUP_DIAL 26`
4. Create `configs/Sky2040/README.md` with pin documentation
5. Test build:
   ```bash
   export GP2040_BOARDCONFIG=Sky2040
   cd build && cmake .. && make -j$(sysctl -n hw.ncpu)
   ```

**Success Criteria**:
- ✅ Builds without errors
- ✅ All buttons respond correctly
- ✅ Display shows properly
- ✅ RGB LEDs work

### Phase 2: MCP23017 Library ⏱️ 2-3 hours
**Tasks**:
1. Create `lib/mcp23017/` directory
2. Implement `mcp23017.h` (header file)
3. Implement `mcp23017.cpp` (driver code)
4. Add to CMakeLists.txt
5. Test I2C communication:
   - Verify address detection (0x20)
   - Test register read/write
   - Verify no conflict with display (0x3C)
6. Create test program to read switches

**Success Criteria**:
- ✅ MCP23017 detected on I2C bus
- ✅ Can read Port A register
- ✅ Display continues working
- ✅ No I2C bus conflicts

### Phase 3: Hardware Assembly ⏱️ 3-4 hours
**Tasks**:
1. Acquire components:
   - MCP23017 chip (DIP-28 or SOIC-28 with adapter)
   - 8× SPST toggle switches OR DIP-8 switch array
   - 10kΩ linear potentiometer
   - Breadboard + jumper wires
   - 0.1μF ceramic capacitor
   - 10kΩ resistor (RESET pull-up)
2. Wire MCP23017 to Sky 2040:
   - Power: VDD→3.3V, VSS→GND
   - I2C: SDA→GP0, SCL→GP1
   - Address: A0/A1/A2→GND (addr 0x20)
   - RESET→3.3V via 10kΩ
   - Add 0.1μF cap across VDD/VSS
3. Wire 8 switches to MCP23017:
   - Each terminal 1→GPA0-7
   - Each terminal 2→GND
4. Wire speed dial:
   - Pin 1→GND
   - Pin 2→GPIO 26
   - Pin 3→3.3V
5. Verify connections:
   - Check I2C with logic analyzer or scope (optional)
   - Test continuity
   - Verify no shorts

**Success Criteria**:
- ✅ MCP23017 powers up
- ✅ I2C communication works
- ✅ All switches toggle cleanly
- ✅ Potentiometer sweeps 0-3.3V
- ✅ Display still works

### Phase 4: Turbo Addon Integration ⏱️ 4-6 hours
**Tasks**:
1. Modify `headers/addons/turbo.h`:
   - Include MCP23017 header
   - Add MCP23017* member to TurboInput class
   - Add I2C configuration constants
2. Modify `src/addons/turbo.cpp`:
   - Initialize MCP23017 in `setup()`
   - Read switches in `process()`
   - Map switch states to button masks
   - Override turboButtonsMask with hardware state
3. Handle I2C bus sharing:
   - Verify display addon initializes I2C first
   - Ensure MCP23017 doesn't re-initialize bus
   - Add mutex if concurrent access needed (unlikely)
4. Rebuild and flash firmware
5. Test each switch independently
6. Test multiple switches simultaneously
7. Test speed dial adjustment
8. Verify no display interference

**Success Criteria**:
- ✅ Each switch controls correct button turbo
- ✅ All 8 switches work independently
- ✅ Speed dial adjusts turbo rate smoothly
- ✅ Display continues working perfectly
- ✅ No I2C bus conflicts or glitches
- ✅ RGB LEDs still functional

### Phase 5: Testing & Validation ⏱️ 2-3 hours
**Hardware Tests**:
1. Individual button turbo (8 tests)
2. Multiple button turbo simultaneously (all 8)
3. Speed dial adjustment while turbo active
4. Long-duration stress test (30+ minutes)
5. Power cycle persistence
6. I2C bus stress test (rapid switch toggling)

**Cross-Platform Tests**:
1. PC (XInput mode)
2. Nintendo Switch
3. PS4/PS5 (if dongle available)
4. Steam Deck (if available)

**Edge Cases**:
1. All 8 switches ON simultaneously
2. Rapid switch toggling (stress I2C)
3. Speed dial at min/max extremes
4. Turbo button press during hardware turbo
5. Display updates during switch reads

**Success Criteria**:
- ✅ All tests pass without errors
- ✅ No I2C bus hangs or timeouts
- ✅ Display remains responsive
- ✅ Consistent behavior across platforms
- ✅ No firmware crashes

### Phase 6: Optimization & Polish ⏱️ 1-2 hours
**Tasks**:
1. Measure I2C read latency
2. Optimize switch polling frequency
3. Add error recovery for I2C failures
4. Implement switch debouncing if needed
5. Add OLED display indicators for active turbo
6. Test and tune turbo speed range

**Success Criteria**:
- ✅ I2C latency <1ms
- ✅ Total polling overhead <1% CPU
- ✅ Robust error handling
- ✅ Clean switch response

### Phase 7: Documentation ⏱️ 1-2 hours
**Tasks**:
1. Update `configs/Sky2040/README.md`:
   - Complete pin mapping table
   - I2C expander wiring diagram
   - Turbo switch wiring
   - Speed dial wiring
   - I2C address configuration
2. Document MCP23017 library API
3. Create build guide for users
4. Add troubleshooting section
5. Take photos of completed build
6. Update CLAUDE.md with final status

**Total Time**: 14-21 hours for complete implementation

---

## Build & Flash Commands

### Build Firmware
```bash
# Set board configuration
export GP2040_BOARDCONFIG=Sky2040

# Navigate to project
cd /Users/fwong/Documents/github/wongfei2009/GP2040-CE

# Create and enter build directory
mkdir -p build && cd build

# Configure build
cmake ..

# Build (use all CPU cores)
make -j$(sysctl -n hw.ncpu)

# Output file: GP2040-CE_X.X.X_Sky2040.uf2
```

### Flash to Device
```bash
# Method 1: Hold BOOTSEL while plugging in USB
# Method 2: Short RUN pin to GND twice quickly

# Copy firmware (device appears as RPI-RP2)
cp GP2040-CE_*_Sky2040.uf2 /Volumes/RPI-RP2/

# Wait for automatic reboot
```

---

## Success Criteria Checklist

### Must Have ✅
- [ ] Sky2040 BoardConfig builds successfully
- [ ] All 18 original buttons work correctly
- [ ] MCP23017 communicates via I2C
- [ ] Speed dial adjusts turbo speed (2-30 shots/sec)
- [ ] All 8 turbo switches control their respective buttons
- [ ] Hardware switches override software turbo
- [ ] Display continues working (no I2C conflict)
- [ ] RGB LEDs still functional
- [ ] No input latency added (<1ms overhead)
- [ ] Firmware stable in all input modes

### Nice to Have 🎯
- [ ] OLED shows active turbo indicators
- [ ] Web configurator shows switch states
- [ ] I2C error recovery implemented
- [ ] Settings persist across power cycles
- [ ] Upstream PR contribution

---

## Technical Notes

### I2C Bus Sharing
**Challenge**: MCP23017 and display share the same I2C bus
**Solution**:
- Display initializes I2C bus first (address 0x3C)
- MCP23017 uses same bus, different address (0x20)
- No mutex needed (both accessed from same thread)
- Turbo addon reads after display updates

**Bus Speed**: 400kHz (fast mode)
- Display: 400kHz
- MCP23017: up to 1.7MHz (we use 400kHz to match)

**Performance Impact**:
- I2C read (8 GPIO): ~50-100µs
- Total overhead: <0.1% of 1ms polling cycle
- No measurable latency increase

### Memory Footprint
- **Code size**: +~4KB (MCP23017 library + integration)
- **RAM usage**: +64 bytes (MCP23017 object + buffers)
- **Flash config**: +16 bytes (I2C config)
- **Total impact**: <1% of available memory

### Hardware Trade-offs

**What We Gain**:
- ✅ Full 8-button hardware turbo control
- ✅ Variable speed dial (2-30 shots/sec)
- ✅ Instant turbo enable/disable
- ✅ Physical tactile feedback
- ✅ No GPIO limitations
- ✅ RGB LEDs preserved
- ✅ Expansion capability (8 more GPIOs on Port B)

**What We Sacrifice**:
- ⚠️ Added complexity (external chip required)
- ⚠️ Slight cost increase (~$1-2 for MCP23017)
- ⚠️ Additional wiring
- ⚠️ Breadboard or custom PCB needed

**Justification**: 
- Option C provides the complete original vision
- I2C expanders are reliable and well-tested
- No compromise on features
- Scalable for future expansion

---

## Troubleshooting Guide

### Issue: MCP23017 not detected on I2C bus
**Diagnostics**:
1. Check I2C address configuration:
   - Verify A0/A1/A2 pins are connected to GND
   - Expected address: 0x20
   - Use I2C scanner to detect devices
2. Verify power connections:
   - VDD connected to 3.3V (NOT 5V!)
   - VSS connected to GND
   - Check voltage with multimeter: ~3.3V
3. Check I2C bus connections:
   - SDA (pin 13) → GP0 (Sky 2040 pin 1)
   - SCL (pin 12) → GP1 (Sky 2040 pin 2)
   - Verify with multimeter for continuity
4. Check RESET pin:
   - Should be pulled HIGH via 10kΩ resistor
   - Never leave floating or connected to GND
5. Add 0.1µF capacitor between VDD and VSS

**Fix**: 
- Double-check all wiring
- Verify 3.3V power (NOT 5V)
- Test with I2C scanner code first

### Issue: Display stops working after adding MCP23017
**Diagnostics**:
1. Check for I2C address conflict
   - Display: 0x3C
   - MCP23017: 0x20
   - Should be different!
2. Verify display still initializes first
3. Check if I2C bus speed matches (400kHz)
4. Look for shorts or loose connections

**Fix**:
- Ensure display addon initializes before turbo addon
- Verify no I2C address conflict
- Add delays between I2C transactions if needed

### Issue: Switches don't enable turbo
**Diagnostics**:
1. Check switch wiring:
   - Terminal 1 → MCP23017 GPA pin
   - Terminal 2 → GND
2. Verify MCP23017 pull-ups enabled (GPPUA = 0xFF)
3. Test switch continuity with multimeter
4. Read GPA register directly to see raw values
5. Check button mask mapping in code

**Fix**:
- Verify active-low logic (LOW = ON)
- Check pull-up configuration
- Test each switch independently

### Issue: Speed dial doesn't change speed
**Diagnostics**:
1. Measure voltage at wiper: should sweep 0-3.3V
2. Check `PIN_SHMUP_DIAL` is set to 26
3. Verify potentiometer wiring (CCW→GND, CW→3.3V)
4. Test ADC reading in web configurator

**Fix**:
- Ensure pot is 10kΩ linear taper
- Verify wiper connection to GPIO 26
- Check for reversed connections

### Issue: I2C bus hangs or times out
**Diagnostics**:
1. Check for short circuits on SDA/SCL lines
2. Verify pull-up resistors present (usually built-in)
3. Check bus capacitance (long wires add capacitance)
4. Look for EMI from RGB LEDs or other sources

**Fix**:
- Keep I2C wires short (<6 inches ideal)
- Add 2.2kΩ-4.7kΩ pull-ups if needed
- Reduce I2C speed to 100kHz if problems persist
- Shield I2C wires if near noisy signals

### Issue: Some switches work, others don't
**Diagnostics**:
1. Test each GPA pin individually
2. Check for cold solder joints
3. Verify switch functionality with multimeter
4. Read MCP23017 GPIOA register to see raw data

**Fix**:
- Re-solder suspect connections
- Replace faulty switches
- Verify pin mapping in code matches hardware

### Issue: Turbo speed inconsistent
**Diagnostics**:
1. Check for noise on ADC input (GPIO 26)
2. Verify 0.1µF capacitor on pot wiper to GND
3. Test pot with multimeter (should be linear)
4. Check for loose wiper connection

**Fix**:
- Add 0.1µF filter cap between wiper and GND
- Use shielded wire for ADC connection
- Replace potentiometer if worn

---

## Component Sourcing

### MCP23017 I2C GPIO Expander
**Vendors**:
- DigiKey: [MCP23017-E/SP](https://www.digikey.com/product-detail/en/MCP23017-E-SP/MCP23017-E-SP-ND/)
- Mouser: Part #579-MCP23017-E/SP
- Adafruit: [I2C Expander Breakout](https://www.adafruit.com/product/732)
- Amazon: Search "MCP23017" (~$5-10 for breakout boards)

**Recommended**: Buy pre-soldered breakout board for easier prototyping

### Other Components
- **10kΩ Potentiometer**: Any linear taper, rotary or slide
- **SPST Switches**: DigiKey, Mouser, or Amazon
- **DIP-8 Switch Array**: Easier than 8 individual switches
- **Breadboard**: Standard 830-point breadboard
- **Jumper Wires**: Male-to-male and male-to-female

---

## Alternative: Custom PCB Design

For a permanent installation, consider designing a custom PCB:

### PCB Features
1. MCP23017 surface-mount footprint
2. 8× switch footprints (through-hole)
3. Potentiometer mounting
4. 40-pin header to plug onto Sky 2040
5. Power regulation and filtering
6. I2C level shifting (if needed)

### PCB Design Tools
- **KiCad**: Free, open-source
- **EasyEDA**: Free, web-based
- **Eagle**: Free for small boards

### PCB Manufacturers
- **JLCPCB**: $2 for 5 boards + assembly service
- **PCBWay**: Similar pricing
- **OSH Park**: Made in USA, $5/sq in

---

## Future Expansion Ideas

### Using MCP23017 Port B (8 additional GPIOs)
The MCP23017 has 8 unused GPIOs on Port B (GPB0-GPB7) available for:

1. **Additional Input Buttons**
   - Macro buttons
   - Profile switching
   - Custom functions

2. **LED Indicators**
   - Per-button turbo status LEDs
   - Speed indicator LEDs
   - Mode indicators

3. **Analog Inputs** (with external ADC)
   - Second speed dial
   - Analog stick deadzone adjustment
   - Trigger sensitivity

4. **Expansion Connector**
   - Breakout header for future add-ons
   - Plugin modules

### Daisy-Chaining Multiple MCP23017s
- Connect up to 8 chips on same I2C bus
- Addresses: 0x20-0x27
- 128 additional GPIO pins possible!

---

## Resources

- **GP2040-CE Docs**: https://gp2040-ce.info
- **Turbo Add-on**: https://gp2040-ce.info/add-ons/turbo/
- **MCP23017 Datasheet**: https://www.microchip.com/wwwproducts/en/MCP23017
- **I2C Bus Spec**: https://www.nxp.com/docs/en/user-guide/UM10204.pdf
- **Discord Support**: https://discord.gg/k2pxhke7q8
- **GitHub Repo**: https://github.com/OpenStickCommunity/GP2040-CE
- **RP2040 Datasheet**: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf

---

## Next Immediate Steps

### Ready to Start
1. ✅ **Decision made**: Option C (I2C expander)
2. ✅ **Documentation complete**: Full implementation plan
3. ⏭️ **Next action**: Order MCP23017 chip + components

### Phase 1 Action Items (After Components Arrive)
```bash
# 1. Create directory
mkdir -p configs/Sky2040

# 2. Copy base config
cp configs/Pico/BoardConfig.h configs/Sky2040/BoardConfig.h

# 3. Edit the file (apply all changes from Phase 1)

# 4. Create MCP23017 library
mkdir -p lib/mcp23017
# (implement mcp23017.h and mcp23017.cpp)

# 5. Test build
export GP2040_BOARDCONFIG=Sky2040
cd build && cmake .. && make -j$(sysctl -n hw.ncpu)
```

**After Phase 1 success** → Assemble hardware (Phase 3) → Integrate software (Phase 4)

---

## Revision History
- **2024-11-12**: Initial comprehensive plan created (Option B)
- **2024-11-12**: Revised to Option C with I2C expander (MCP23017)
- **2024-11-12**: Complete implementation guide with all phases documented

---

## macOS Build Instructions

Building the firmware from source on macOS requires a specific setup to resolve dependency conflicts. The following steps have been verified to produce a successful build.

### Prerequisites

1.  **Xcode Command Line Tools**: Install by running `xcode-select --install` in your terminal.
2.  **Homebrew**: The macOS package manager. If not installed, get it from [brew.sh](https://brew.sh/).
3.  **ARM GCC Toolchain**: Install via Homebrew Cask:
    ```bash
    brew install --cask gcc-arm-embedded
    ```

### Build Steps

The build process involves manually downloading the correct Pico SDK version before running CMake and Make.

1.  **Clone the Correct Pico SDK Version**

    The project requires Pico SDK version **2.1.1**. Clone it into the project's root directory:
    ```bash
    git clone --branch 2.1.1 https://github.com/raspberrypi/pico-sdk.git pico-sdk-2.1.1
    ```

2.  **Initialize SDK Submodules**

    Navigate into the newly created SDK directory and initialize its submodules (which include the compatible Mbed TLS library):
    ```bash
    cd pico-sdk-2.1.1
    git submodule update --init --recursive
    cd ..
    ```

3.  **Configure and Build the Firmware**

    This single command chain will clean any previous build, set the required environment variables, configure the project with CMake, and compile it with Make.

    Run this from the root of the `GP2040-CE` project directory:
    ```bash
    rm -rf build && \
    mkdir build && \
    cd build && \
    export SDKROOT=$(xcrun --sdk macosx --show-sdk-path) && \
    PICO_SDK_PATH=$(pwd)/../pico-sdk-2.1.1 GP2040_BOARDCONFIG=Pico cmake -DPICO_SDK_FETCH_FROM_GIT=OFF .. && \
    make -j$(sysctl -n hw.ncpu)
    ```

    - `rm -rf build`: Cleans up any previous build attempts.
    - `export SDKROOT=...`: Explicitly sets the path to the macOS SDK, which fixes issues with compiling host-side Python tools.
    - `PICO_SDK_PATH=...`: Points the build system to our manually downloaded SDK.
    - `GP2040_BOARDCONFIG=Pico`: Sets the target board. Change `Pico` to your desired board if different.
    - `PICO_SDK_FETCH_FROM_GIT=OFF`: Prevents the build system from automatically downloading another SDK.
    - `make -j...`: Compiles the firmware using all available CPU cores.

    Upon completion, the firmware file (e.g., `GP2040-CE_0.0.0_Pico.uf2`) will be located in the `build` directory.
