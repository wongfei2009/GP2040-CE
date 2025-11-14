# Repository Guidelines

## Project Structure & Module Organization
Firmware sources live in `src/` with shared headers in `headers/`; keep board-agnostic logic there and isolate platform quirks through the addon interfaces in `src/addons/` and `headers/addons/`. Every supported PCB has `configs/<Board>/BoardConfig.h` plus optional pinout notes—clone an existing folder when adding hardware. Reusable drivers (TinyUSB forks, MCP23017, etc.) live in `lib/`, while generated protocol buffers originate in `proto/`. The React/Vite configurator is contained in `www/`; its generated TypeScript (`www/src_gen/`) is overwritten by `npm run build-proto`. Treat `build/` as disposable CMake output.

## Build, Test, and Development Commands
Firmware build (macOS/Linux shown):
```bash
mkdir -p build && cd build
PICO_SDK_PATH=$(pwd)/../pico-sdk-2.1.1 GP2040_BOARDCONFIG=Pico cmake .. -DPICO_SDK_FETCH_FROM_GIT=OFF
make -j$(sysctl -n hw.ncpu)
```
Switch `GP2040_BOARDCONFIG` (e.g., `Sky2040`) to target another board and rerun CMake after cleaning `build/`. The resulting `.uf2` appears in `build/` and can be copied to the RP2040 boot drive. Web configurator workflow:
```bash
cd www
npm install
npm run dev          # Vite + express stub for quick UI checks
npm run build        # bundles frontend and runs makefsdata
npm run lint         # ESLint
npm run format -- --check
```
Use `npm run build-proto` when protobuf enums change and `npm run check-locale` before touching translations.

## Coding Style & Naming Conventions
The repo enforces `.editorconfig`: tabs with size 2 for C/C++, LF endings, and spaces for Markdown/YAML. C++ targets C++17; prefer descriptive CamelCase for types, snake_case for functions, and `kConstant` prefixes already in use. Keep board folders PascalCase (`configs/Pico`, `configs/Sky2040`) and name new entry points `BoardConfig.h`. Frontend code uses ESLint + Prettier (single quotes) and TypeScript strictness; do not edit generated files in `src_gen/`.

## Testing Guidelines
No automated unit suite exists, so validate firmware on physical hardware: flash the new `.uf2`, confirm USB descriptors, button matrices, LEDs, and any addons you touched (display, turbo, RGB). Document manual steps in the PR if the scenario is hard to reproduce. For the web app, run `npm run lint`, `npm run check-locale`, and exercise affected screens via `npm run dev`; include screenshots when UX is impacted. When introducing protobuf or settings changes, ensure the configurator and firmware both build from a clean tree.

## Commit & Pull Request Guidelines
Commits follow short, imperative subjects (`Add macOS build instructions`, `Remove legacy pinout`). Group related changes and mention impacted boards in the subject or body. Reference issues with `Fixes #123` where applicable and avoid committing build outputs. PRs should summarize the motivation, include flashing/testing notes, and link any hardware docs or assets. Add screenshots or serial logs for UI and USB changes, and make sure CI-friendly commands (`cmake`, `make`, `npm run build`) succeed before requesting review.

## Configuration & Security Tips
Set `PICO_SDK_PATH` explicitly to the checked-in `pico-sdk-2.1.1` and keep toolchains up to date (`gcc-arm-embedded`). Never hard-code secrets or Wi-Fi credentials in configs; store personal settings outside tracked files. When adding addons that share buses (I²C, SPI), reuse existing initializers to avoid conflicts and document reserved pins in the relevant `configs/<Board>/README.md`.
