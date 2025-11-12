# Sky 2040 GP2040-CE Turbo Enhancement Project

## Quick Summary
**Goal**: Add hardware-based turbo controls to Sky 2040 joystick
- ✅ Analog turbo speed dial (potentiometer, 2-30 shots/sec)
- ✅ 8 per-button turbo switches (B1-B4, L1-L2, R1-R2)
- **Strategy**: Build on existing Pico config (98% compatible)
- **GPIO Required**: 9 pins (1 ADC + 8 digital) - ALL AVAILABLE ✅
- **Trade-off**: RGB LEDs disabled to free GPIO 28 for R2 switch

## Project Location
- **Fork**: `/Users/fwong/Documents/github/wongfei2009/GP2040-CE`
- **Upstream**: OpenStickCommunity/GP2040-CE

---

## Sky 2040 Configuration (Verified from Backup)

### Base Configuration
- **Board Type**: Pico-variant (98% match)
- **Only Difference**: GPIO 2/3 swapped for UP/DOWN
- **Source**: `gp2040ce_backup_20251112061231421.gp2040`

### Pin Mappings

#### Buttons (Active-Low with Pull-Up)
| Button | GPIO | Function | Notes |
|--------|------|----------|-------|
| B1 (A/Cross) | 6 | Face button | Turbo-capable |
| B2 (B/Circle) | 7 | Face button | Turbo-capable |
| B3 (X/Square) | 10 | Face button | Turbo-capable |
| B4 (Y/Triangle) | 11 | Face button | Turbo-capable |
| L1 | 13 | Shoulder | Turbo-capable |
| R1 | 12 | Shoulder | Turbo-capable |
| L2 | 9 | Trigger | Turbo-capable |
| R2 | 8 | Trigger | Turbo-capable |
| Up | 3 | D-pad | ⚠️ Swapped vs Pico |
| Down | 2 | D-pad | ⚠️ Swapped vs Pico |
| Left | 5 | D-pad | |
| Right | 4 | D-pad | |
| Select (S1) | 16 | System | |
| Start (S2) | 17 | System | |
| L3 | 18 | Stick | |
| R3 | 19 | Stick | |
| Home (A1) | 20 | PS/Guide | |
| Capture (A2) | 21 | Capture | |
| Turbo | 14 | Turbo toggle | Existing button |

#### Peripherals
- **Display (I2C)**: SDA=GPIO0, SCL=GPIO1, Addr=0x3C
- **RGB LEDs**: ~~Data=GPIO28~~ **DISABLED** (GPIO freed for R2 switch)
- **Turbo Button**: GPIO14 (LED disabled)

---

## Hardware Implementation

### Turbo Speed Dial (Analog Potentiometer)
**Component**: 10kΩ linear potentiometer (rotary or slide)

**Wiring**:
```
Potentiometer:
  Pin 1 (CCW/Left)  → GND
  Pin 2 (Wiper)     → GPIO 26 (ADC0)
  Pin 3 (CW/Right)  → 3.3V
```

**Software**: Already supported via `pinShmupDial` in existing turbo addon

### Per-Button Turbo Switches (8× SPST Toggles)
**Component**: 8× SPST toggle switches OR 1× DIP-8 switch array

**Pin Assignment**:
```
GPIO 15 → B1 (A/Cross)    turbo enable
GPIO 22 → B2 (B/Circle)   turbo enable
GPIO 23 → B3 (X/Square)   turbo enable
GPIO 24 → B4 (Y/Triangle) turbo enable
GPIO 25 → L1              turbo enable
GPIO 27 → R1              turbo enable
GPIO 28 → R2              turbo enable  ⚠️ RGB LEDs sacrificed
GPIO 29 → L2              turbo enable
```

**Wiring per Switch** (Active-Low):
```
Each Switch:
  Terminal 1 → GPIO pin (from table above)
  Terminal 2 → GND
  (Internal pull-up resistor enabled in firmware)

Logic:
  Switch CLOSED (ON)  = GPIO reads LOW  = Turbo ENABLED
  Switch OPEN (OFF)   = GPIO reads HIGH = Normal operation
```

### Complete GPIO Allocation
| GPIO | Original Function | New Function | Status |
|------|-------------------|--------------|--------|
| 26 | Unassigned | Speed Dial (ADC) | ✅ Used |
| 15 | Turbo LED (disabled) | B1 Switch | ✅ Repurposed |
| 22 | Unassigned | B2 Switch | ✅ Used |
| 23 | Unassigned | B3 Switch | ✅ Used |
| 24 | Unassigned | B4 Switch | ✅ Used |
| 25 | Unassigned | L1 Switch | ✅ Used |
| 27 | Unassigned | R1 Switch | ✅ Used |
| 28 | RGB LED Data | R2 Switch | ⚠️ **RGB DISABLED** |
| 29 | Unassigned | L2 Switch | ✅ Used |

**Total Used**: 9 GPIOs (1 ADC + 8 digital) = Perfect fit! ✅

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

// Disable RGB LEDs (GPIO 28 freed for R2 switch)
#define BOARD_LEDS_PIN -1  // Disabled

// Turbo speed dial
#define PIN_SHMUP_DIAL 26  // ADC0

// Per-button turbo switches
#define TURBO_SWITCH_ENABLED 1
#define PIN_TURBO_SWITCH_B1 15  // B1 (A/Cross)
#define PIN_TURBO_SWITCH_B2 22  // B2 (B/Circle)
#define PIN_TURBO_SWITCH_B3 23  // B3 (X/Square)
#define PIN_TURBO_SWITCH_B4 24  // B4 (Y/Triangle)
#define PIN_TURBO_SWITCH_L1 25  // L1
#define PIN_TURBO_SWITCH_R1 27  // R1
#define PIN_TURBO_SWITCH_R2 28  // R2 (was RGB LED)
#define PIN_TURBO_SWITCH_L2 29  // L2
```

### Phase 2: Extend Turbo Add-on
**Files**: `src/addons/turbo.cpp`, `headers/addons/turbo.h`

**New Structure in turbo.h**:
```cpp
// Per-button turbo switch configuration
struct TurboSwitchConfig {
    int8_t switchPins[8];        // GPIO pins for 8 switches
    uint16_t buttonMasks[8];     // Button masks (GAMEPAD_MASK_B1, etc.)
};
```

**Modified Logic in turbo.cpp**:
```cpp
// In setup() - Initialize switch GPIOs
void TurboInput::setup() {
    // ... existing setup code ...
    
    #ifdef TURBO_SWITCH_ENABLED
    // Initialize hardware turbo switches (active-low with pull-up)
    const int8_t switchPins[] = {
        PIN_TURBO_SWITCH_B1, PIN_TURBO_SWITCH_B2, 
        PIN_TURBO_SWITCH_B3, PIN_TURBO_SWITCH_B4,
        PIN_TURBO_SWITCH_L1, PIN_TURBO_SWITCH_R1,
        PIN_TURBO_SWITCH_R2, PIN_TURBO_SWITCH_L2
    };
    
    for (uint8_t i = 0; i < 8; i++) {
        if (switchPins[i] >= 0) {
            gpio_init(switchPins[i]);
            gpio_set_dir(switchPins[i], GPIO_IN);
            gpio_pull_up(switchPins[i]);  // Active-low
        }
    }
    #endif
}

// In process() - Read hardware switches
void TurboInput::process() {
    #ifdef TURBO_SWITCH_ENABLED
    // Read hardware turbo switches (active-low)
    uint16_t hardwareTurboMask = 0;
    const int8_t switchPins[] = {
        PIN_TURBO_SWITCH_B1, PIN_TURBO_SWITCH_B2,
        PIN_TURBO_SWITCH_B3, PIN_TURBO_SWITCH_B4,
        PIN_TURBO_SWITCH_L1, PIN_TURBO_SWITCH_R1,
        PIN_TURBO_SWITCH_R2, PIN_TURBO_SWITCH_L2
    };
    const uint16_t buttonMasks[] = {
        GAMEPAD_MASK_B1, GAMEPAD_MASK_B2,
        GAMEPAD_MASK_B3, GAMEPAD_MASK_B4,
        GAMEPAD_MASK_L1, GAMEPAD_MASK_R1,
        GAMEPAD_MASK_R2, GAMEPAD_MASK_L2
    };
    
    for (uint8_t i = 0; i < 8; i++) {
        if (switchPins[i] >= 0 && !gpio_get(switchPins[i])) {
            hardwareTurboMask |= buttonMasks[i];
        }
    }
    
    // Hardware switches override software turbo settings
    turboButtonsMask = hardwareTurboMask;
    #endif
    
    // Continue with existing turbo flicker logic
    // (speed dial already supported via pinShmupDial)
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
   - Disable RGB: `#define BOARD_LEDS_PIN -1`
   - Add turbo switch pin definitions
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

### Phase 2: Hardware Preparation ⏱️ 2-3 hours
**Tasks**:
1. Acquire components:
   - 8× SPST toggle switches OR 1× DIP-8 switch array
   - 1× 10kΩ linear potentiometer (rotary or slide)
   - Jumper wires / breadboard for prototyping
2. Wire speed dial:
   - Pin 1 → GND
   - Pin 2 (wiper) → GPIO 26
   - Pin 3 → 3.3V
3. Wire turbo switches (all active-low):
   - Switch terminal 1 → respective GPIO (15, 22-25, 27-29)
   - Switch terminal 2 → GND
4. Verify connections:
   - Test continuity with multimeter
   - Check for shorts to 3.3V or 5V
   - Confirm switches toggle cleanly

**Success Criteria**:
- ✅ All switches toggle cleanly
- ✅ Potentiometer sweeps full range (0-3.3V)
- ✅ No electrical shorts
- ✅ Connections secure

### Phase 3: Speed Dial Integration ⏱️ 1 hour
**Tasks**:
1. Verify `PIN_SHMUP_DIAL 26` in BoardConfig
2. Rebuild firmware
3. Flash to Sky2040
4. Test in web configurator:
   - Navigate to Add-Ons → Turbo
   - Rotate dial, observe speed change
5. Calibrate range to 2-30 shots/sec

**Success Criteria**:
- ✅ Turning dial changes turbo speed in real-time
- ✅ Full 2-30 shots/sec range accessible
- ✅ No jitter or noise in ADC reading
- ✅ Speed updates without lag

### Phase 4: Turbo Switches Implementation ⏱️ 4-6 hours
**Tasks**:
1. Add switch definitions to BoardConfig (see Phase 1)
2. Modify `headers/addons/turbo.h`:
   - Add `TurboSwitchConfig` structure
   - Add switch pin array declarations
3. Modify `src/addons/turbo.cpp`:
   - Add GPIO initialization in `setup()`
   - Add switch polling in `process()`
   - Implement hardware mask override logic
4. Rebuild and flash firmware
5. Test each switch independently:
   - Toggle switch ON → button auto-fires at dial speed
   - Toggle switch OFF → button operates normally
6. Test multiple switches simultaneously
7. Verify turbo works in all input modes (XInput, Switch, etc.)
8. Performance test: measure input latency impact

**Success Criteria**:
- ✅ Each switch controls correct button's turbo
- ✅ Switches override software turbo settings
- ✅ No input latency added (<1ms)
- ✅ Switches work in all game modes
- ✅ Clean enable/disable on toggle
- ✅ Firmware stable under all conditions

### Phase 5: Testing & Validation ⏱️ 1-2 hours
**Hardware Tests**:
1. Individual button turbo (8 tests)
2. Multiple button turbo simultaneously
3. Speed dial adjustment while turbo active
4. Long-duration stress test (30+ minutes)
5. Power cycle persistence

**Cross-Platform Tests**:
1. PC (XInput mode)
2. Nintendo Switch
3. PS4/PS5 (if dongle available)
4. Steam Deck (if available)

**Edge Cases**:
1. All 8 switches ON simultaneously
2. Rapid switch toggling
3. Speed dial at min/max extremes
4. Turbo button press during hardware turbo

**Success Criteria**:
- ✅ All tests pass without errors
- ✅ No crashes or firmware hangs
- ✅ Consistent behavior across platforms

### Phase 6: Documentation ⏱️ 1-2 hours
**Tasks**:
1. Update `configs/Sky2040/README.md`:
   - Complete pin mapping table
   - Turbo switch wiring diagram
   - Speed dial wiring diagram
   - RGB LED trade-off explanation
2. Document turbo switch behavior
3. Create usage guide for end users
4. Add troubleshooting section
5. Take photos of completed build
6. Update this CLAUDE.md with final status

**Success Criteria**:
- ✅ Complete documentation
- ✅ Clear wiring diagrams
- ✅ Usage instructions
- ✅ Troubleshooting guide

**Total Time**: 10-15 hours for complete implementation

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
- [ ] Speed dial adjusts turbo speed (2-30 shots/sec)
- [ ] All 8 turbo switches control their respective buttons
- [ ] Hardware switches override software turbo
- [ ] No input latency added (<1ms overhead)
- [ ] Firmware stable in all input modes
- [ ] RGB LEDs cleanly disabled
- [ ] I2C display still functional

### Nice to Have 🎯
- [ ] Web configurator shows switch states
- [ ] OLED displays active turbo indicators
- [ ] Settings persist across power cycles
- [ ] Upstream PR contribution

---

## Technical Notes

### Performance Impact
- **GPIO reads**: ~8 digital reads per cycle = negligible (<0.5µs)
- **ADC read**: 1 read per cycle = ~12µs (already in use)
- **Total overhead**: <1% of 1ms input polling cycle
- **Expected latency**: No measurable increase

### Memory Footprint
- **Code size**: +~2KB (switch logic)
- **RAM usage**: +16 bytes (pin arrays)
- **Flash config**: +9 bytes (pin definitions)
- **Total impact**: Negligible (<1% of available)

### Hardware Trade-offs
**What We Gain**:
- ✅ Full hardware turbo control (all 8 buttons)
- ✅ Instant turbo enable/disable (no button combos)
- ✅ Physical tactile feedback
- ✅ No accidental turbo activation
- ✅ Independent control per button

**What We Lose**:
- ❌ RGB LED visual feedback (GPIO 28 sacrificed)
- ❌ Per-button LED indicators
- ❌ Aesthetic lighting effects

**Justification**: For competitive gaming, hardware turbo control provides more value than RGB aesthetics. Visual feedback can be provided via OLED display if needed.

---

## Troubleshooting Guide

### Issue: Switch doesn't enable turbo
**Diagnostics**:
1. Check switch wiring (terminal 1 → GPIO, terminal 2 → GND)
2. Verify GPIO number in BoardConfig matches physical wiring
3. Test switch continuity with multimeter
4. Check if GPIO is properly initialized (add debug logging)

**Fix**: Verify `gpio_pull_up()` is called and switch is active-low

### Issue: Speed dial doesn't change speed
**Diagnostics**:
1. Measure voltage at wiper: should sweep 0-3.3V
2. Check `PIN_SHMUP_DIAL` is set to 26 (ADC0)
3. Verify potentiometer is connected correctly (CCW→GND, CW→3.3V)
4. Test ADC reading in web configurator

**Fix**: Ensure potentiometer wiring is correct and not reversed

### Issue: Multiple buttons fire when one switch enabled
**Diagnostics**:
1. Check for electrical shorts between GPIO pins
2. Verify button masks in code match physical buttons
3. Test each switch independently

**Fix**: Check pin assignments and ensure no crossed wires

### Issue: Turbo speed too fast/slow
**Diagnostics**:
1. Check speed dial range in web configurator
2. Verify ADC is reading full 0-3.3V range
3. Check turbo shot count settings

**Fix**: Calibrate speed dial range or adjust turbo shot count

### Issue: Firmware won't build
**Diagnostics**:
1. Verify `GP2040_BOARDCONFIG=Sky2040` is set
2. Check all files are in correct directories
3. Review compiler errors for syntax issues

**Fix**: Ensure BoardConfig.h has no syntax errors, all pins defined

---

## Next Immediate Steps

### Ready to Start
1. ✅ **Decision made**: Option B (8 switches, no RGB)
2. ✅ **Documentation complete**: This guide ready
3. ⏭️ **Next action**: Create `configs/Sky2040/BoardConfig.h`

### Phase 1 Action Items
```bash
# 1. Create directory
mkdir -p configs/Sky2040

# 2. Copy base config
cp configs/Pico/BoardConfig.h configs/Sky2040/BoardConfig.h

# 3. Edit the file (apply all changes from Phase 1)

# 4. Test build
export GP2040_BOARDCONFIG=Sky2040
cd build && cmake .. && make -j$(sysctl -n hw.ncpu)
```

**After Phase 1 success** → Proceed to hardware wiring (Phase 2)

---

## Resources

- **GP2040-CE Docs**: https://gp2040-ce.info
- **Turbo Add-on**: https://gp2040-ce.info/add-ons/turbo/
- **Discord Support**: https://discord.gg/k2pxhke7q8
- **GitHub Repo**: https://github.com/OpenStickCommunity/GP2040-CE
- **RP2040 Datasheet**: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
- **GPIO Reference**: https://gp2040-ce.info/controller-build/wiring/

---

## Revision History
- **2024-11-12**: Initial comprehensive plan created
- **2024-11-12**: Compacted and optimized for Option B implementation
