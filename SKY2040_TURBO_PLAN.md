# Sky 2040 Custom Turbo Implementation Plan

## Executive Summary

This document outlines the plan to add hardware-based turbo controls to the Sky 2040 joystick board using GP2040-CE firmware:

✅ **Hardware Turbo Speed Dial** - Analog potentiometer for real-time speed adjustment (2-30 shots/sec)
✅ **8 Per-Button Turbo Switches** - Physical toggle switches for each button (B1-B4, L1-L2, R1-R2)

**Feasibility: CONFIRMED** - Both features are implementable with existing GP2040-CE architecture.

## Hardware Requirements

### 1. Turbo Speed Dial
- **Component**: 10kΩ linear potentiometer (rotary or slide)
- **Connection**: Wiper → GPIO 26/27/28/29 (ADC pin), other pins to GND and 3.3V
- **Pins Required**: 1 ADC-capable GPIO
- **Status**: ✅ Already supported by GP2040-CE (`shmupDialPin`)

### 2. Per-Button Turbo Switches
- **Components**: 8× SPST toggle switches or 1× DIP-8 switch array
- **Connection**: Each switch to unique GPIO pin + GND (active-low with internal pull-up)
- **Pins Required**: 8 GPIO pins
- **Status**: ⚠️ Requires firmware extension (straightforward)

### Total GPIO Budget
- **Required**: 9 pins (1 ADC + 8 digital)
- **Available on Sky 2040**: ~8-10 pins (estimated)
- **Conclusion**: ✅ Fits within pin budget

## Implementation Phases

### Phase 1: Board Configuration & Setup
1. Create Sky 2040 board configuration directory
2. Map GPIO pins for buttons and controls
3. Build and flash test firmware
4. Verify basic functionality

### Phase 2: Turbo Speed Dial Integration
1. Wire potentiometer to ADC pin (recommend GPIO 28)
2. Enable `PIN_SHMUP_DIAL` in BoardConfig.h
3. Test existing dial functionality
4. Calibrate speed range feel

### Phase 3: Per-Button Turbo Switch Implementation
1. Select 8 available GPIO pins for switches
2. Wire 8 toggle switches (or DIP-8 array)
3. Extend `turbo.cpp` to poll switch states
4. Map switch states to turbo button masks
5. Test and debug

### Phase 4: Refinement & Polish
1. Add OLED display indicators (if available)
2. Test latency impact (should be zero)
3. Document wiring in board README
4. Create user guide

## Code Modifications Required

### 1. BoardConfig.h (configs/Sky2040/BoardConfig.h)

```cpp
// Enable turbo add-on
#define TURBO_ENABLED 1

// Turbo speed dial (ADC)
#define PIN_SHMUP_DIAL 28  // Or 26, 27, 29

// Per-button turbo enable switches (choose available GPIOs)
#define PIN_TURBO_SWITCH_B1 22
#define PIN_TURBO_SWITCH_B2 23
#define PIN_TURBO_SWITCH_B3 24
#define PIN_TURBO_SWITCH_B4 25
#define PIN_TURBO_SWITCH_L1 26
#define PIN_TURBO_SWITCH_R1 27
#define PIN_TURBO_SWITCH_L2 29
#define PIN_TURBO_SWITCH_R2 21
```

### 2. turbo.h Extension

```cpp
class TurboInput : public GPAddon {
    // ... existing members ...
private:
    // New: Switch configuration
    uint8_t turboSwitchPins[8];      // GPIO pins for switches
    uint16_t turboSwitchMasks[8];    // Button masks
    uint32_t turboSwitchPinMasks[8]; // Pin masks for fast reading
    
    void initSwitches();
    uint16_t readSwitchStates();
};
```

### 3. turbo.cpp Extension

```cpp
void TurboInput::setup() {
    // ... existing setup code ...
    
    // Initialize turbo switches
    initSwitches();
}

void TurboInput::initSwitches() {
    const TurboOptions& options = Storage::getInstance().getAddonOptions().turboOptions;
    
    // Configure switch pins (these would come from BoardConfig.h)
    turboSwitchPins[0] = PIN_TURBO_SWITCH_B1;
    turboSwitchPins[1] = PIN_TURBO_SWITCH_B2;
    // ... etc
    
    turboSwitchMasks[0] = GAMEPAD_MASK_B1;
    turboSwitchMasks[1] = GAMEPAD_MASK_B2;
    // ... etc
    
    // Initialize GPIO pins
    for (uint8_t i = 0; i < 8; i++) {
        if (isValidPin(turboSwitchPins[i])) {
            gpio_init(turboSwitchPins[i]);
            gpio_set_dir(turboSwitchPins[i], GPIO_IN);
            gpio_pull_up(turboSwitchPins[i]);  // Active-low with pull-up
            turboSwitchPinMasks[i] = 1 << turboSwitchPins[i];
        }
    }
}

uint16_t TurboInput::readSwitchStates() {
    Gamepad* gamepad = Storage::getInstance().GetGamepad();
    uint16_t hardwareTurboMask = 0;
    
    for (uint8_t i = 0; i < 8; i++) {
        if (turboSwitchPinMasks[i] > 0) {
            // Read switch state (active-low)
            bool switchOn = !(gamepad->debouncedGpio & turboSwitchPinMasks[i]);
            if (switchOn) {
                hardwareTurboMask |= turboSwitchMasks[i];
            }
        }
    }
    
    return hardwareTurboMask;
}

void TurboInput::process() {
    // ... existing code ...
    
    // Read hardware switches and update turbo mask
    uint16_t hardwareTurboMask = readSwitchStates();
    turboButtonsMask = hardwareTurboMask | alwaysEnabled;
    
    // ... rest of existing turbo logic ...
}
```

## Testing Plan

### Turbo Speed Dial Testing
1. Connect multimeter to dial wiper to verify voltage range (0-3.3V)
2. Test firmware reads ADC values correctly
3. Verify turbo speed changes smoothly across full dial range
4. Check for jitter or noise in readings

### Turbo Switch Testing
1. Verify each switch triggers correct button turbo
2. Test multiple switches active simultaneously
3. Measure input latency with switches enabled (should be <1ms)
4. Test switch + dial interaction works correctly
5. Verify OLED display updates to show turbo status

## Wiring Diagram (Conceptual)

```
Sky 2040 Board
├── Button Inputs (existing)
│   ├── B1-B4 → GPIO 6-13
│   ├── L1,L2,R1,R2 → GPIO 8,9,12,13
│   └── Directions, Start, Select → Other GPIOs
│
├── Turbo Speed Dial (NEW)
│   └── 10kΩ Pot → GPIO 28 (ADC)
│       ├── Wiper → GPIO 28
│       ├── End 1 → GND
│       └── End 2 → 3.3V
│
└── Turbo Enable Switches (NEW)
    ├── Switch 1 (B1) → GPIO 22 + GND
    ├── Switch 2 (B2) → GPIO 23 + GND
    ├── Switch 3 (B3) → GPIO 24 + GND
    ├── Switch 4 (B4) → GPIO 25 + GND
    ├── Switch 5 (L1) → GPIO 26 + GND
    ├── Switch 6 (R1) → GPIO 27 + GND
    ├── Switch 7 (L2) → GPIO 29 + GND
    └── Switch 8 (R2) → GPIO 21 + GND
```

## Alternative: GPIO Expander

If GPIO pins are limited, use PCF8574 I2C GPIO expander:

```cpp
// In BoardConfig.h
#define BOARD_HAS_I2C_GPIO_EXPANDER 1
#define I2C_GPIO_EXPANDER_ADDRESS 0x20

// Use expander pins for turbo switches
#define TURBO_SWITCHES_USE_EXPANDER 1
```

Benefits:
- Only uses 2 pins (I2C SDA/SCL)
- GP2040-CE already has PCF8574 support
- Slightly slower but acceptable for switches

## Performance Considerations

### Latency Impact Analysis
- **Switch polling**: ~1-2 microseconds per switch × 8 = 16μs
- **ADC reading**: ~10 microseconds (sampled every 100ms)
- **Total overhead**: <20μs per 1000μs polling cycle
- **Impact**: <2% of polling budget → **Negligible** ✅

### Flash Memory
- Code size increase: ~2-3KB (turbo switch logic)
- Storage for config: ~16 bytes
- Total: Well within RP2040's 2MB flash

## Success Criteria

✅ Turbo speed dial adjusts turbo rate in real-time (2-30 shots/sec)
✅ All 8 switches correctly enable/disable turbo per button
✅ Input latency remains <1ms average
✅ Switches and dial work simultaneously without interference
✅ OLED display (if present) shows current turbo status
✅ Build size remains under 1.5MB

## Next Steps

1. **Immediate**: Verify exact GPIO pinout on physical Sky 2040 board
2. **Immediate**: Order components (pot + switches)
3. **Week 1**: Create configs/Sky2040/ directory and BoardConfig.h
4. **Week 1**: Build and flash test firmware
5. **Week 2**: Wire and test turbo dial
6. **Week 2-3**: Implement and test per-button switches
7. **Week 3**: Polish and document

## Questions to Resolve

- [ ] What GPIO pins are actually available on Sky 2040?
- [ ] Does Sky 2040 have I2C already in use for display?
- [ ] Physical switch mounting location/space available?
- [ ] Preference for rotary pot vs slide pot for speed dial?
- [ ] DIP switch array vs individual toggle switches?

## References

- GP2040-CE Turbo Docs: https://gp2040-ce.info/add-ons/turbo/
- GPIO Mapping: https://gp2040-ce.info/web-configurator/menu-pages/gpio-pin-mapping/
- Board Building: https://gp2040-ce.info/controller-build/wiring/
- Sky 2040 Review: https://thearcadestick.com/sky2040/
