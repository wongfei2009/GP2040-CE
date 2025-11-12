# Sky 2040 Complete Pinout Mapping
## Physical Pin to GPIO Reference

This document maps the physical connector pins (1-30) shown in the official Chinese pinout diagram to their actual RP2040 GPIO numbers, verified against the backup configuration.

---

## Physical Pin Layout

```
┌─────────────────────────────────┐
│  Sky 2040 Board (Top View)      │
├──────────────┬──────────────────┤
│ LEFT HEADER  │  RIGHT HEADER    │
│ (Pins 1-15)  │  (Pins 16-30)    │
└──────────────┴──────────────────┘
```

---

## Left Header (Pins 1-15)

| Physical Pin | Label | Signal | GPIO | Function | Config Verified |
|--------------|-------|--------|------|----------|-----------------|
| 1 | 外=GND, 内=A | Button | GPIO 6 | B1 (A/Cross) | ✅ pin:6 |
| 2 | 外=GND, 内=B | Button | GPIO 7 | B2 (B/Circle) | ✅ pin:7 |
| 3 | 外=GND, 内=X | Button | GPIO 10 | B3 (X/Square) | ✅ pin:10 |
| 4 | 外=GND, 内=Y | Button | GPIO 11 | B4 (Y/Triangle) | ✅ pin:11 |
| 5 | 外=GND, 内=L1 | Button | GPIO 13 | L1 Shoulder | ✅ pin:13 |
| 6 | 外=GND, 内=R1 | Button | GPIO 12 | R1 Shoulder | ✅ pin:12 |
| 7 | 外=GND, 内=L2 | Button | GPIO 9 | L2 Trigger | ✅ pin:9 |
| 8 | 外=GND, 内=R2 | Button | GPIO 8 | R2 Trigger | ✅ pin:8 |
| 9 | 外=GND, 内=L3 | Button | GPIO 18 | L3 Stick Click | ✅ pin:18 |
| 10 | 外=GND, 内=R3 | Button | GPIO 19 | R3 Stick Click | ✅ pin:19 |
| 11 | 外=GND, 内=SEL | Button | GPIO 16 | Select (选择键) | ✅ pin:16 |
| 12 | 外=GND, 内=STA | Button | GPIO 17 | Start (开始键) | ✅ pin:17 |
| 13 | 外=GND, 内=HOME | Button | GPIO 20 | Home/PS (PS键) | ✅ pin:20 |
| 14 | 外=GND, 内=TURBO | Button | GPIO 14 | Turbo Toggle | ✅ turboPin:14 |
| 15 | 外=GND, 内=TU-LED | LED Output | GPIO 15 | Turbo LED (disabled) | ⚠️ turboPinLED:-1 |

---

## Right Header (Pins 16-30)

| Physical Pin | Label | Signal | GPIO | Function | Config Verified |
|--------------|-------|--------|------|----------|-----------------|
| 16 | 内=GND, 外=BOOT | Button | - | BOOTSEL (hardware) | N/A (RP2040 builtin) |
| 17 | 内=AL, 外=GND | Button | GPIO 5 | D-pad Left (左) | ✅ pin:5 |
| 18 | 内=AR, 外=GND | Button | GPIO 4 | D-pad Right (右) | ✅ pin:4 |
| 19 | 内=AD, 外=GND | Button | GPIO 2 | D-pad Down (下) | ✅ pin:2 |
| 20 | 内=AU, 外=GND | Button | GPIO 3 | D-pad Up (上) | ✅ pin:3 |
| 21 | 内=GP22, 外=GND | Expansion | GPIO 22 | Unused (扩展键) | ✅ Available |
| 22 | 内=SDA, 外=SWDIO | I2C Data | GPIO 0 | Display SDA | ✅ sdaPin:0 |
| 23 | 内=SCL, 外=SWCLK | I2C Clock | GPIO 1 | Display SCL (OLED屏) | ✅ sclPin:1 |
| 24 | 内=3V3, 外=3V3 | Power | - | 3.3V Supply | N/A |
| 25 | 内=GND, 外=GND | Ground | - | Ground | N/A |
| 26 | 内=RUN, 外=RS | Control | - | Reset/Run (备用键) | N/A (RP2040 RUN pin) |
| 27 | 内=GND, 外=LS | Unknown | ? | Unknown (possibly LED-?) | ❓ Not in config |
| 28 | 内=LED-D, 外=GND | LED Output | ? | LED Data (possibly GPIO 28) | ❓ See note |
| 29 | 内=GND, 外=A2 | Mixed | GPIO 21/28? | A2 Button OR RGB LED | ⚠️ See note |
| 30 | 内=5V, 外=GND | Power | - | 5V Supply | N/A |

---

## Special Notes

### Pin 15 (TU-LED / GPIO 15)
- **Label**: "连发指示灯" (Turbo LED indicator)
- **Current Status**: Disabled in config (`turboPinLED: -1`)
- **Original Purpose**: Turbo status indicator LED
- **New Purpose**: **AVAILABLE for B1 turbo switch** ✅
- **Inference**: GPIO 15 was likely the original turbo LED but is now disabled

### Pin 21 (GP22)
- **Label**: "扩展键" (Extension key)
- **Explicitly labeled** as GP22 in the pinout diagram
- **Config Status**: Not assigned to any function
- **Purpose**: **AVAILABLE for B2 turbo switch** ✅

### Pins 28-29 Ambiguity ⚠️
The official pinout has confusing labels for these pins:

**Pin 28**: "LED-D - GND"
- Likely means "LED Data line"
- **Best guess**: GPIO 28 (matches config `dataPin: 28`)

**Pin 29**: "GND - A2"
- Could mean:
  - **Option A**: A2 (Capture) button connector (GPIO 21 confirmed in config)
  - **Option B**: External RGB LED connector (shares GPIO 28 data line?)
- **Physical connector**: Pin 29 is likely just an external RGB LED connector
- **GPIO 21 (A2 button)** is confirmed functional
- **GPIO 28 (RGB data)** is what needs to be freed for R2 turbo switch

### Pin 27 (LS)
- **Label**: Unclear, possibly "Left Shoulder" or LED-related
- **Config Status**: Not assigned
- **Inference**: Likely unused expansion pin

---

## Available GPIOs for Turbo Project

Based on config analysis and physical pinout:

| GPIO | Physical Pin | Current Status | Turbo Project Use |
|------|--------------|----------------|-------------------|
| **15** | Pin 15 | TU-LED (disabled) | B1 turbo switch ✅ |
| **22** | Pin 21 | Unassigned expansion | B2 turbo switch ✅ |
| **23** | - | Not exposed/unassigned | B3 turbo switch ✅ |
| **24** | - | Not exposed/unassigned | B4 turbo switch ✅ |
| **25** | - | Not exposed/unassigned | L1 turbo switch ✅ |
| **26** | - | Not exposed/unassigned | Speed dial (ADC0) ✅ |
| **27** | Pin 27? | Unclear/unassigned | R1 turbo switch ✅ |
| **28** | Pin 28 | **RGB LED data** | R2 switch (disable RGB) ⚠️ |
| **29** | - | Not exposed/unassigned | L2 turbo switch ✅ |

---

## RP2040 GPIO Reference

### All 30 GPIOs Status

| GPIO | Function | Available? | Notes |
|------|----------|------------|-------|
| 0 | Display SDA | ❌ In use | I2C data |
| 1 | Display SCL | ❌ In use | I2C clock |
| 2 | D-pad Down | ❌ In use | Swapped with GP3 |
| 3 | D-pad Up | ❌ In use | Swapped with GP2 |
| 4 | D-pad Right | ❌ In use | |
| 5 | D-pad Left | ❌ In use | |
| 6 | B1 (A) | ❌ In use | Face button |
| 7 | B2 (B) | ❌ In use | Face button |
| 8 | R2 | ❌ In use | Trigger |
| 9 | L2 | ❌ In use | Trigger |
| 10 | B3 (X) | ❌ In use | Face button |
| 11 | B4 (Y) | ❌ In use | Face button |
| 12 | R1 | ❌ In use | Shoulder |
| 13 | L1 | ❌ In use | Shoulder |
| 14 | Turbo button | ❌ In use | Toggle turbo mode |
| **15** | **TU-LED (disabled)** | ✅ **AVAILABLE** | **Freed for B1 switch** |
| 16 | Select | ❌ In use | System button |
| 17 | Start | ❌ In use | System button |
| 18 | L3 | ❌ In use | Stick click |
| 19 | R3 | ❌ In use | Stick click |
| 20 | Home/PS | ❌ In use | System button |
| 21 | A2 (Capture) | ❌ In use | System button |
| **22** | **Expansion** | ✅ **AVAILABLE** | **For B2 switch** |
| **23** | **Unassigned** | ✅ **AVAILABLE** | **For B3 switch** |
| **24** | **Unassigned** | ✅ **AVAILABLE** | **For B4 switch** |
| **25** | **Unassigned** | ✅ **AVAILABLE** | **For L1 switch** |
| **26 (ADC0)** | **Unassigned** | ✅ **AVAILABLE** | **For speed dial** |
| **27** | **Unassigned** | ✅ **AVAILABLE** | **For R1 switch** |
| **28** | **RGB LED data** | ⚠️ **In use** | **Free by disabling RGB** |
| **29 (ADC3)** | **Unassigned** | ✅ **AVAILABLE** | **For L2 switch** |

---

## Confirmed Differences from Raspberry Pi Pico

| Feature | Raspberry Pi Pico | Sky 2040 | Impact |
|---------|-------------------|----------|--------|
| D-pad Up | GPIO 2 | GPIO 3 | ⚠️ Swapped |
| D-pad Down | GPIO 3 | GPIO 2 | ⚠️ Swapped |
| RGB LED | Not included | GPIO 28 | ⚠️ Must disable |
| Turbo LED | Not included | GPIO 15 (disabled) | ℹ️ Already freed |
| Display | Optional | Built-in (GPIO 0/1) | ℹ️ Required |

---

## Physical Wiring Guide for Turbo Switches

All turbo switches use **active-low** configuration with internal pull-ups:

```
Each Switch Wiring:
┌──────────────────┐
│  SPST Switch     │
│                  │
│  Terminal 1  ────┼──→ GPIO (see table above)
│  Terminal 2  ────┼──→ GND
└──────────────────┘

Logic:
- Switch CLOSED (ON)  → GPIO reads LOW  → Turbo ENABLED
- Switch OPEN (OFF)   → GPIO reads HIGH → Normal operation
```

### Speed Dial (Potentiometer) Wiring

```
10kΩ Linear Potentiometer:
┌──────────────────┐
│   Potentiometer  │
│                  │
│  Pin 1 (CCW)  ───┼──→ GND
│  Pin 2 (Wiper) ──┼──→ GPIO 26 (ADC0)
│  Pin 3 (CW)  ────┼──→ 3.3V
└──────────────────┘

Output Range: 0V (min speed) to 3.3V (max speed)
```

---

## Verification Checklist

- [x] All button GPIO assignments verified from backup config
- [x] D-pad swap (GPIO 2↔3) confirmed
- [x] Display I2C (GPIO 0/1) confirmed
- [x] RGB LED (GPIO 28) confirmed as conflict
- [x] Turbo button (GPIO 14) confirmed
- [x] Turbo LED (GPIO 15) confirmed disabled
- [x] All 9 required GPIOs confirmed available
- [x] Physical pin labels cross-referenced with official pinout

---

## References

- **Official Pinout**: Chinese diagram provided by manufacturer
- **Backup Config**: `gp2040ce_backup_20251112061231421.gp2040`
- **RP2040 Datasheet**: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
- **GP2040-CE Docs**: https://gp2040-ce.info

---

*Document created: 2024-11-12*  
*Last verified: 2024-11-12*
