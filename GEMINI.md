# GP2040-CE Project Overview

This document provides a high-level overview of the GP2040-CE project, intended to be used as a context for AI-assisted development.

## Project Overview

GP2040-CE is a multi-platform gamepad firmware for the Raspberry Pi Pico and other RP2040-based microcontrollers. It is a C/C++ project built with CMake and the Raspberry Pi Pico SDK.

The firmware has a multi-core architecture:
- **Core 0:** Runs the main application logic, including gamepad state management, addon processing, and system event handling.
- **Core 1:** Handles I/O, including input drivers and addon processing.

A key feature of GP2040-CE is its addon system, which allows for extending the firmware's functionality with plugins. Addons can be processed on either core.

The project also includes a web-based configuration interface, which is a React/TypeScript application built with Vite. This web interface communicates with the firmware using Protocol Buffers.

The codebase is modular:
- **Core Logic:** `src/`
- **Board-specific Configs:** `configs/`
- **Hardware Drivers:** `lib/`
- **Headers:** `headers/`
- **Add-ons:** `src/addons/` and `headers/addons/`

## Building and Running

The project is built using CMake. The web interface is built using npm.

### Firmware (General)

To build the firmware, you will need to have the Raspberry Pi Pico SDK set up. The build process is as follows:

1.  **Configure CMake:**
    ```bash
    mkdir build
    cd build
    cmake .. -DGP2040_BOARDCONFIG=<board_name>
    ```
    Replace `<board_name>` with the name of the target board (e.g., `Pico`).

2.  **Build the firmware:**
    ```bash
    make
    ```

This will produce a `.uf2` file in the `build` directory, which can be flashed to the RP2040 board.

### Firmware (macOS)

1.  **Install Xcode CLI:** `xcode-select --install`
2.  **Install Homebrew:** https://brew.sh/
3.  **Install ARM GCC:** `brew install --cask gcc-arm-embedded`
4.  **Clone Pico SDK 2.1.1:** `git clone --branch 2.1.1 https://github.com/raspberrypi/pico-sdk.git pico-sdk-2.1.1`
5.  **Initialize SDK Submodules:** `cd pico-sdk-2.1.1 && git submodule update --init --recursive && cd ..`
6.  **Build:**
    ```sh
    rm -rf build && mkdir build && cd build
    export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
    PICO_SDK_PATH=$(pwd)/../pico-sdk-2.1.1 GP2040_BOARDCONFIG=Pico cmake -DPICO_SDK_FETCH_FROM_GIT=OFF ..
    make -j$(sysctl -n hw.ncpu)
    ```
7.  **Flash:** Copy the `.uf2` file to the RPI-RP2 drive after entering bootloader mode.

### Web Interface

The web interface is located in the `www` directory. To build it, you will need to have Node.js and npm installed.

1.  **Install dependencies:**
    ```bash
    cd www
    npm install
    ```

2.  **Build the web interface:**
    ```bash
    npm run build
    ```

This will build the web interface and run the `makefsdata.js` script, which prepares the web assets to be embedded in the firmware.

## Key Architectural Patterns

- **BoardConfig:** Each board has a `BoardConfig.h` in `configs/<Board>/` defining pin mappings, features, and hardware quirks. Use this for new hardware support.
- **Add-ons:** Optional features (e.g., turbo, display) are implemented as add-ons. See `src/addons/` and `headers/addons/` for extension points.
- **I2C Bus Sharing:** Multiple peripherals (e.g., display, MCP23017) share the I2C bus. Always check for address conflicts and initialize the display first.
- **Turbo Hardware Integration:** For advanced turbo (e.g., Sky2040), use the MCP23017 I2C expander. See `lib/mcp23017/`, `headers/addons/turbo.h`, and `src/addons/turbo.cpp` for integration patterns.

## Hardware: Sky 2040 Turbo Enhancement Project

A significant hardware enhancement project is documented in `CLAUDE.md`. The goal is to add hardware-based turbo controls to the Sky 2040 joystick.

- **Features:**
    - Analog turbo speed dial (potentiometer, 2-30 shots/sec)
    - 8 per-button turbo switches (B1-B4, L1-L2, R1-R2)
- **Strategy:** Use an I2C GPIO expander (MCP23017) for the switches.
- **GPIO Required:** 1 ADC (for the speed dial) + I2C bus (shared with the display).
- **Hardware:** MCP23017 chip, potentiometer, and 8 switches.

The `CLAUDE.md` file contains detailed information on the hardware implementation, including wiring diagrams, component lists, and software implementation details.

## Development Conventions

- **Pin mapping and hardware quirks** are always handled in the board config, not in core logic.
- **All new hardware drivers** should be placed in `lib/` and exposed via headers.
- **Add-ons** must not re-initialize shared buses (e.g., I2C) if already initialized by another add-on.
- **Turbo add-on:** Hardware switches override software turbo settings. Use active-low logic for turbo switches.
- **Testing:** Use the web configurator and physical device for validation. Automated tests are not present; manual hardware validation is required.
- **Coding Style:** The project uses the `.editorconfig` file to enforce a consistent coding style.
- **C++ Standard:** C++17
- **Contributions:** Pull requests are welcome for enhancements, bug fixes, and documentation updates.
- **Web Interface:** The web interface is a React application written in TypeScript. It uses Bootstrap for styling, Formik for forms, and i18next for internationalization.

## Example: Adding a New Board

1.  Copy an existing board config (e.g., `configs/Pico/BoardConfig.h`).
2.  Update pin mappings, labels, and feature flags.
3.  Add a `README.md` in the new board's config directory documenting pinout and quirks.
4.  Build with `GP2040_BOARDCONFIG=<NewBoard>`.

## Troubleshooting

- **I2C issues:** Check address, wiring, and bus speed (400kHz recommended). The display must initialize I2C first.
- **Turbo not working:** Verify switch wiring (active-low), pull-ups, and pin mapping in both hardware and code.
- **Build errors:** Ensure correct Pico SDK version and toolchain. Clean the build directory if switching boards.