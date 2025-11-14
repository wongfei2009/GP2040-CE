# Copilot Coding Agent Instructions for GP2040-CE

## Project Overview
- **GP2040-CE** is multi-platform gamepad firmware for RP2040-based boards (e.g., Raspberry Pi Pico), supporting PC, PlayStation, Switch, Xbox, and more.
- The codebase is modular: core logic in `src/`, board-specific configs in `configs/`, hardware drivers in `lib/`, and headers in `headers/`.
- Add-ons (e.g., turbo, display) are in `src/addons/` and `headers/addons/`.
- Extensive hardware support is managed via per-board configs and conditional compilation.

## Build & Flash Workflow
- **macOS build (recommended):**
  1. Install Xcode CLI: `xcode-select --install`
  2. Install Homebrew: https://brew.sh/
  3. Install ARM GCC: `brew install --cask gcc-arm-embedded`
  4. Clone Pico SDK 2.1.1 to `pico-sdk-2.1.1/` and run its submodule init.
  5. Build:
     ```sh
     rm -rf build && mkdir build && cd build
     export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
     PICO_SDK_PATH=$(pwd)/../pico-sdk-2.1.1 GP2040_BOARDCONFIG=Pico cmake -DPICO_SDK_FETCH_FROM_GIT=OFF ..
     make -j$(sysctl -n hw.ncpu)
     ```
  6. Flash: Copy `.uf2` file to the RPI-RP2 drive after entering bootloader mode.
- **Board selection:** Set `GP2040_BOARDCONFIG` to match your target in `configs/` (e.g., `Sky2040`, `Pico`).

## Key Architectural Patterns
- **BoardConfig:** Each board has a `BoardConfig.h` in `configs/<Board>/` defining pin mappings, features, and hardware quirks. Use this for new hardware support.
- **Add-ons:** Optional features (e.g., turbo, display) are implemented as add-ons. See `src/addons/` and `headers/addons/` for extension points.
- **I2C Bus Sharing:** Multiple peripherals (e.g., display, MCP23017) share the I2C bus. Always check for address conflicts and initialize the display first.
- **Turbo Hardware Integration:** For advanced turbo (e.g., Sky2040), use the MCP23017 I2C expander. See `lib/mcp23017/`, `headers/addons/turbo.h`, and `src/addons/turbo.cpp` for integration patterns.

## Project-Specific Conventions
- **Pin mapping and hardware quirks** are always handled in the board config, not in core logic.
- **All new hardware drivers** should be placed in `lib/` and exposed via headers.
- **Add-ons** must not re-initialize shared buses (e.g., I2C) if already initialized by another add-on.
- **Turbo add-on**: Hardware switches override software turbo settings. Use active-low logic for turbo switches.
- **Testing:** Use the web configurator and physical device for validation. Automated tests are not present; manual hardware validation is required.

## Example: Adding a New Board
1. Copy an existing board config (e.g., `configs/Pico/BoardConfig.h`).
2. Update pin mappings, labels, and feature flags.
3. Add a `README.md` in the new board's config directory documenting pinout and quirks.
4. Build with `GP2040_BOARDCONFIG=<NewBoard>`.

## Troubleshooting
- **I2C issues:** Check address, wiring, and bus speed (400kHz recommended). Display must initialize I2C first.
- **Turbo not working:** Verify switch wiring (active-low), pull-ups, and pin mapping in both hardware and code.
- **Build errors:** Ensure correct Pico SDK version and toolchain. Clean build directory if switching boards.

## References
- [Project Docs](https://gp2040-ce.info)
- [Turbo Add-on](https://gp2040-ce.info/add-ons/turbo/)
- [Discord Support](https://discord.gg/k2pxhke7q8)

---
For major changes, review `CLAUDE.md` for detailed hardware and integration notes, especially for Sky2040 and turbo enhancements.
