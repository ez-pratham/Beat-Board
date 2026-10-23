# BEAT BOARD ⭐
### Handheld ESP32 Cyber-Console Synthesizer & Drum Machine (version P.V.01)

**BEAT BOARD** is a handheld music device and synthesizer with a cyber-retro aesthetic, built around the dual-core **ESP32** microcontroller. It features four gold-plated capacitive touch pads for finger drumming and expressive brass solos, an interactive 3.5" color display showing real-time audio waveforms and sequencer grids, a dedicated $2 \times 2$ Master Control Pad for navigation, and an onboard MAX98357A I2S Class-D amplifier driving an external 3W full-range acoustic speaker.

Pressing the **MODE** button brings up an interactive on-screen menu allowing you to instantly switch between an **808 Drum Machine** (with a live 16-step visual sequencer) and a **4-Valve Acoustic Trumpet Brass Synth** (complete with real-time moving valve pistons and soundwave flares)!

---

## Photos & Screenshots

### 3D Render (Assembled Cyber-Console)
![BEAT BOARD 3D Render](./docs/renders/music_pad_3d_render_v2_3.jpg)

### Top View (Silkscreen Doodles & Controls)
![Top View](./docs/renders/pcb_render_top.png)

### Bottom View (ROG "BEAT BOARD" & Punk Doodles)
![Bottom View](./docs/renders/pcb_render_bottom.png)

### Isometric Perspective View
![Perspective View](./docs/renders/pcb_render_perspective.png)

### Schematic
![Schematic Diagram](./docs/images/schematic.png)  
*(Vector PDF available at [docs/images/schematic.pdf](./docs/images/schematic.pdf) | Vector SVG at [docs/images/schematic_diagram.svg](./docs/images/schematic_diagram.svg))*

### PCB Board Layout
#### Top Layer Layout (Orthogonal SPI/I2S Routing & Ground Plane)
![PCB Layout Top](./docs/images/pcb_layout_top.png)

#### Bottom Layer Layout (Power Rails & Ground Plane)
![PCB Layout Bottom](./docs/images/pcb_layout_bottom.png)

### Wiring Diagram (Off-Board Hand-Wired Connections)
![Wiring Diagram](./docs/images/wiring_diagram.png)

*(Photos of the real assembled board will be added here as soon as my PCBs arrive in the mail!)*

---

## How It Works

### How the parts fit on the console

I laid out every section of the $200 \times 120\text{ mm}$ carrier PCB to make the console as ergonomic and comfortable as possible for fast finger drumming and trumpet solos:

1. **Center Top (Display)**:
   - A **3.5" Color TFT LCD module (480x320 SPI)** sits elevated in the upper center of the board.
   - Displays live audio waveforms, 16-step drum sequencer grids, BPM tempo, battery status, and the interactive instrument selection menu.

2. **Center Core (Microcontroller)**:
   - An **ESP32-WROOM-32 DevKit** microcontroller sits in dual female socket headers directly below the display.
   - Its USB-C port faces downwards at the bottom edge, making firmware flashing and USB power charging convenient.
   - Ample clearance surrounds both sides of the ESP32 to eliminate heat buildup and mechanical crowding.

3. **Left Wing (Master Navigation & Drum Pads)**:
   - **Master Control Pad (Top Left)**: A $2 \times 2$ tactile button grid (`UP`, `DOWN`, `SELECT`, `MODE`) lets your left thumb navigate menus, change kits, shift octaves, or mute the horn without lifting your hands.
   - **Left Touch Pads (Bottom Left)**: Two large $24 \times 24\text{ mm}$ gold-plated capacitive touch pads (`PAD 1: KICK / Bb3` and `PAD 2: SNARE / D4`) driven by two compact red TTP223 daughterboards.

4. **Dedicated Top-Right Audio Bay**:
   - The **MAX98357A I2S 3W Class-D amplifier module** and green **KF301 2-pin speaker screw terminal block** are placed in the isolated top-right corner.
   - Twisted red and black speaker wires leave cleanly from the top-right edge to a 3W cavity speaker, completely removed from your playing hands.

5. **Right Wing (100% Unobstructed Touch Controls)**:
   - The outermost elements on the right edge are `PAD 3: HI-HAT / F4` and `PAD 4: CLAP / Bb4`.
   - Your fingers and thumbs have direct, unhindered access to the pads with zero obstacles or hurdles in the playing zone.

6. **Bottom Power Subsystem**:
   - Features a **TP4056 USB-C LiPo charger**, an aluminum **SPDT slide power switch**, a **JST-PH 2.0mm connector**, and a **KF301 screw terminal** for universal 3.7V LiPo battery packs.

---

## Bill of Materials (BOM)

| Item | Qty | Supplier | Cost / Item (USD) | Total Cost (USD) | Link |
| :--- | :---: | :--- | :---: | :---: | :--- |
| **ESP32 38-Pin Development Board (Dual-Core WiFi/BT)** | 1 | Robu.in | $5.35 | $5.35 | [Robu.in](https://robu.in/product/esp32-38pin-development-board-wifibluetooth-ultra-low-power-consumption-dual-core/) |
| **3.5" TFT Touch Screen Display Module** | 1 | Robu.in | $11.30 | $11.30 | [Robu.in](https://robu.in/product/3-5-inch-tft-touch-screen-module-mega-2560-r3/) |
| **Adafruit MAX98357A I2S 3W Class-D Amplifier Breakout** | 1 | Robu.in | $5.95 | $5.95 | [Robu.in](https://robu.in/product/adafruit-max98357a-i2s-3w-class-d-amplifier-breakout-board/) |
| **4Ω 40mm Diameter 3W Full-Range Audio Speaker** | 1 | Robogears | $0.81 | $0.81 | [Robogears](https://www.robogears.in/shop/012020-4ohm-40mm-diameter-3w-full-range-audio-speaker-2986) |
| **TTP223 Touch Key Sensor Modules (Pack of 2)** | 2 | Robu.in | $0.23 | $0.46 | [Robu.in](https://robu.in/product/ttp223-touch-key-module-2pcs/) |
| **6x6x5mm Tactile Push Button Switches (Pack of 10)** | 1 | Robu.in | $0.23 | $0.23 | [Robu.in](https://robu.in/product/6x6x5-tactile-push-button-switch/) |
| **TP4056 1A Li-ion Battery Charger Module (Type-C)** | 1 | Robu.in | $0.23 | $0.23 | [Robu.in](https://robu.in/product/tp4056-1a-li-ion-lithium-battery-charging-module-with-current-protection-type-c/) |
| **1250mAh PCM Protected Micro Li-Po Battery (3.7V)** | 1 | Robu.in | $5.46 | $5.46 | [Robu.in](https://robu.in/product/1250mah-pcm-protected-micro-li-po-battery/) |
| **Subminiature SPDT Slide Power Switch** | 1 | Robu.in | $0.18 | $0.18 | [Robu.in](https://robu.in/product/1-month-warranty-257/) |
| **KF301 2-Pin 5.08mm Pitch Screw Terminal Blocks (Pack of 5)** | 1 | Robu.in | $0.42 | $0.42 | [Robu.in](https://robu.in/product/kf301-2-pin-5-08mm-pitch-plug-in-screw-terminal-block-connector-pack-of-5/) |
| **JST 2-Pin Male Connector (2mm Pitch)** | 1 | Robu.in | $0.06 | $0.06 | [Robu.in](https://robu.in/product/jst-sh-2-pin-connector-2mm-pitch/) |
| **2.54mm 1x40-Pin Female Single Row Header Strips (Pack of 10)** | 1 | Robu.in | $1.43 | $1.43 | [Robu.in](https://robu.in/product/2-54mm-1x40-pin-female-single-row-header-strip-pack-of-10/) |
| **Custom BEAT BOARD Carrier PCB Manufacturing (5 pcs)** | 5 | Robu.in | $7.39 | $36.97 | [Robu.in](https://robu.in/product/online-pcb-manufacturing-service/) |
| **Total Project Cost** | **18** | — | — | **$68.85** | — |

> **Cost-Saving Tip**: If ordering PCBs via JLCPCB or PCBWay, the 5-board PCB manufacturing cost drops to ~$2.00, bringing the total project cost down to **~$33.88 USD**!

*Downloadable spreadsheet format is available at [BOM.csv](./BOM.csv).*

---

## How to Build

### 1. Solder Low-Profile Components
- Insert and solder the four tactile push-buttons (`SW1`, `SW2`, `SW3`, `SW4`) in the Master Control Pad area on the left wing.
- Solder the SPDT slide power switch (`SW_PWR`) at the bottom edge.

### 2. Solder Connectors & Screw Terminals
- Solder the two KF301 green 2-pin screw terminal blocks:
  - `J_SPK` in the top-right audio bay.
  - `J_BAT_TERM` in the bottom power bay.
- Solder the 2-pin JST-PH battery header (`J_BAT_PIN`).

### 3. Solder Female Socket Headers
- Cut and solder standard 2.54mm female header strips:
  - `J_ESP_L` and `J_ESP_R`: Two 19-pin female rows for the ESP32 DevKit.
  - `J_TFT`: One 9-pin female row for the 3.5" TFT display.
  - `J_AMP`: One 7-pin female row for the MAX98357A I2S amplifier module.
  - `J_TP1` to `J_TP4`: Four 3-pin female rows for the TTP223 touch sensor boards.
  - `J_CHG`: One 4-pin female row for the TP4056 USB-C charging module.

### 4. Mount M3 Standoffs
- Screw four M3x12mm brass standoffs into corner mounting holes `H1`, `H2`, `H3`, and `H4` to create sturdy desk feet and protect the bottom solder joints.

### 5. Plug In Modular Breakout Boards
- Plug the ESP32 DevKit into its dual socket headers (ensure the USB-C port points downwards toward the bottom edge).
- Plug the 3.5" TFT display into `J_TFT`.
- Plug the MAX98357A amplifier into `J_AMP`.
- Plug the four red TTP223 touch boards into `J_TP1` through `J_TP4`.
- Plug the TP4056 module into `J_CHG`.

### 6. Hand-Wired External Connections
Only two off-board components require hand wiring:
1. **Speaker Wiring**: Strip ~5mm from the red and black wires of the 4Ω 3W speaker. Insert the red wire into `J_SPK` Pin 1 (SPK+) and the black wire into `J_SPK` Pin 2 (SPK-), then tighten down the screw terminals.
2. **Battery Wiring**: Connect your 3.7V LiPo battery's red wire to Pin 1 (+) and black wire to Pin 2 (-) on `J_BAT_TERM`, or plug directly into `J_BAT_PIN`.
3. Slide `SW_PWR` to the **ON** position and start making beats!

---

## Software

The firmware supports both physical hardware and online simulation via Wokwi and standard modern browsers.

### Arduino IDE Upload Instructions
1. Open the Arduino IDE.
2. Under **Tools > Board > esp32**, select **ESP32 Dev Module**.
3. Under **Sketch > Include Library > Manage Libraries...**, install:
   - `Adafruit GFX Library`
   - `Adafruit ILI9341` (or `Adafruit_ILI9488`)
4. Open the firmware sketch:
   - **Multi-Engine Firmware**: [firmware/music_pad_wokwi.ino](./firmware/music_pad_wokwi.ino)
   - **Dedicated Trumpet Synthesizer**: [firmware/standalone_trumpet_synth/standalone_trumpet_synth.ino](./firmware/standalone_trumpet_synth/standalone_trumpet_synth.ino)
5. Select your ESP32's COM port and click **Upload**!

### Interactive Web Simulator
You can also preview and test the complete audio and UI engine directly in your web browser by opening [music_pad_simulator.html](./music_pad_simulator.html).

---

## KiCad & Production Files

All hardware design files are verified with KiCad 10.0 and have achieved a verified **0 DRC violations, 0 unconnected items** rating:

- **KiCad Project File**: [kicad_project/music_pad.kicad_pro](./kicad_project/music_pad.kicad_pro)
- **KiCad Schematic**: [kicad_project/music_pad.kicad_sch](./kicad_project/music_pad.kicad_sch)
- **KiCad PCB Layout**: [kicad_project/music_pad.kicad_pcb](./kicad_project/music_pad.kicad_pcb)
- **Production Gerber Archive**: [gerbers.zip](./gerbers.zip) *(Ready for instant submission to JLCPCB, PCBWay, or Robu)*
- **Bill of Materials Spreadsheet**: [BOM.csv](./BOM.csv)
- **Official DRC Verification Report**: [DRC.rpt](./DRC.rpt)
- **Engineering Journal**: [JOURNAL.md](./JOURNAL.md)
