# Engineering Journal // BEAT BOARD (version P.V.01)
## ESP32 Handheld Cyber-Console Music Pad

**Designer**: Pratham Prajapati  
**Platform**: ESP32-WROOM-32 (Dual Core 240MHz, 520KB SRAM)  
**PCB Dimensions**: 200.0 mm x 120.0 mm (FR-4, 2-Layer, 1.6mm thickness, Matte Black Solder Mask)  
**Audio Architecture**: MAX98357A I2S 3W Class-D Mono DAC + Hardware Buzzer Feedback  
**Display**: 3.5" 480x320 SPI Color TFT (ILI9488 / ILI9341 Driver)  
**Firmware Engines**: 808 Vintage Drum Sequencer + 4-Valve Acoustic Trumpet Brass Synth  

---

## 📅 Timeline & Milestone Log

### Milestone 1: Concept & Initial Carrier Architecture
- **Objective**: Create an all-in-one handheld musical instrument workstation using an ESP32 microcontroller that integrates physical touch pads, real-time waveform display, and low-latency audio output.
- **Initial Layout**:
  - Center: ESP32 DevKit mounted in female socket headers.
  - Top: 3.5" SPI TFT display module.
  - Left Wing: 2 large capacitive touch pads (Kick and Snare) + UP/DOWN buttons.
  - Right Wing: 2 large capacitive touch pads (Hi-Hat and Clap) + SELECT/MODE buttons + MAX98357A I2S amplifier breakout.
- **Problem Identified**: The user noticed during ergonomics review that placing the audio amplifier, speaker terminal, and SELECT/MODE buttons on the right wing created a physical hurdle. The player's right hand was obstructed when tapping PAD 3 and PAD 4.

---

### Milestone 2: Ergonomic Overhaul & Right-Wing Clearing (v2.3)
- **User Request**: *"The speaker module, the audio amplifier, and the speaker output on the right wing is blocking my finger sensors. Move MODE and SELECT to the left side, and transfer the audio amplifier and speaker to the top-right corner so the touch sensor is the outermost element on the PCB with no hurdle between the sensors and fingers."*
- **Engineering Changes**:
  1. **Master Control Pad (Left Wing)**:
     - Consolidated all four navigation buttons into a $2 \times 2$ grid on the left wing:
       - Row 1: `SW1` (UP / OCT+) and `SW2` (DOWN / OCT-)
       - Row 2: `SW3` (SELECT / KIT) and `SW4` (MODE / MENU)
     - Allows complete single-thumb navigation of menus, kit changes, tempo, and octave shifts.
  2. **Dedicated Top-Right Audio Bay**:
     - Relocated `J_AMP` (MAX98357A I2S DAC), `J_AMP_OUT` (header), and `J_SPK` (KF301 screw terminal) to $(X \in [166, 204\text{ mm}], Y \in [18, 44\text{ mm}])$ in the top-right corner.
     - Red and black twisted speaker wires route cleanly out the top-right edge, completely isolated from finger playing areas.
  3. **Outermost Touch Pad Freedom**:
     - Both `PAD 3: HI-HAT` and `PAD 4: CLAP` now sit at the extreme outer edge ($X = 166.0$ to $196.0\text{ mm}$), offering $100\%$ zero-barrier finger tapping.

---

### Milestone 3: KiCad Design Rule Check (DRC) Zero-Defect Resolution
- **Challenge**: The initial layout had 58 KiCad DRC violations (silkscreen overlaps with pads, annular ring clearance warnings, missing solder mask clearances).
- **Systematic Resolution**:
  1. Adjusted silkscreen text clearances: Moved all silkscreen labels $>0.5\text{ mm}$ away from component pads.
  2. Enforced KiCad board text minimum height ($\ge 0.85\text{ mm}$, thickness $\ge 0.12\text{ mm}$) to satisfy DRC font constraints.
  3. Verified back silkscreen mirroring: Fixed all `B.Silkscreen` text items to include `.SetMirrored(True)`, resolving `[nonmirrored_text_on_back_layer]`.
  4. Trace rerouting: Routed the 7 TFT SPI signals, 3 I2S audio lines, 4 touch sensor signals, and dual power rails using Manhattan orthogonal tracks on `F.Cu` and `B.Cu` with solid dual ground fills.
- **DRC Verification Result**:
  - `Found 0 violations`
  - `Found 0 unconnected pads` (105 / 105 pads 100% connected)
  - `Found 0 footprint errors`

---

### Milestone 4: Dual-Engine Firmware (808 Drum Machine + 4-Valve Trumpet)
- **User Request**: Add an interactive menu on the screen accessible via the MODE button. Use UP/DOWN buttons to scroll and select between the 808 Drum Machine and a Trumpet brass synth.
- **Firmware Implementation**:
  - Built a modal state engine:
    - Mode 0: `808 DRUM MACHINE` (16-step visual sequencer, Kick, Snare, Closed Hat, Hand Clap, BPM adjustment 60-180, dual kit styles).
    - Mode 1: `4-VALVE TRUMPET BRASS SYNTH` (Bb concert pitch acoustic lead: Bb3 Root, D4 3rd, F4 5th, Bb4 Octave).
  - Piston Animation: Created a graphic visualizer on the TFT display where each brass valve visibly presses down in real time with acoustic soundwaves radiating from the trumpet's bell.
  - Tone Shaping: Implemented Harmon Mute vs. Open Brass synthesis using pitch-bend harmonics and sharp transient bursts.
  - Tested in Wokwi simulator and built an interactive standalone web simulator (`music_pad_simulator.html`).

---

### Milestone 5: Vector Doodles & ROG Typography
- **Inspiration**: Two reference sheets containing musical instruments (headphones, cassette tape, vinyl record, microphone, 5-line staff) and punk/rock graffiti (Flying-V electric guitar, rock-on hand gesture, mohawk skull, flaming heart, thunderbolts).
- **Execution**:
  - Developed custom Python vector drawing algorithms inside KiCad to generate clean silkscreen vector lines without overlapping any pads.
  - Front Side: Cassette tape (`808 TAPE`), Over-ear monitoring headphones, 5-line musical staff with floating notes ($\text{♩, ♫}$), Flying-V electric guitar (`SOLO // LEAD`), Trumpet drawing, and equalizer bars.
  - Back Side Hero Branding: In the exact center of the back, rendered `BEAT BOARD` in authentic Republic of Gamers (ROG) font (sharp $45^\circ$ cut chamfers, $0.34\text{ mm}$ line weight) with subscript `"version P.V.01"` and cyber-shield frame.
  - Back Side Wings: Rock-on hand gesture ($\backslash\text{m/}$), Flaming heart tattoo, Vinyl LP record with turntable tonearm, and Rebellious punk skull with 5-spike mohawk.
  - **Zero-DRC Maintained**: Verified with `kicad-cli pcb drc` with 0 errors and 0 warnings.
  - **3D Renders**: Produced 1920x1080 ray-traced renders for Top, Bottom, and Isometric Perspective views.

---

## 🛠️ Design Files Structure

```
music_pad/
├── README.md                           # Master Project Documentation & Guide
├── JOURNAL.md                          # Engineering Development Journal
├── BOM.csv                             # Bill of Materials Spreadsheet (with links)
├── gerbers.zip                         # Complete Gerber & Drill Archive for Fabrication
├── kicad_project/
│   ├── music_pad.kicad_pro             # KiCad Project File
│   ├── music_pad.kicad_sch             # Complete Schematic File
│   ├── music_pad.kicad_pcb             # Zero-DRC Production Carrier PCB
│   └── DRC.rpt                         # Official DRC Clean Verification Report
├── firmware/
│   ├── standalone_trumpet_synth/       # Dedicated 4-Valve Trumpet Firmware (.ino)
│   ├── music_pad_wokwi.ino             # Multi-Engine Live Wokwi Firmware
│   └── music_pad_firmware/             # Complete ESP32 Production Source
└── docs/
    ├── renders/                        # High-Resolution 3D Ray-Traced Renders
    │   ├── pcb_render_top.png          # Top Silkscreen & Layout View
    │   ├── pcb_render_bottom.png       # Bottom View (BEAT BOARD ROG + Art)
    │   ├── pcb_render_perspective.png  # 3D Isometric View
    │   └── music_pad_3d_render_v2_3.jpg# Photorealistic Assembled Render
    └── images/
        ├── schematic.pdf               # Full Schematic in Vector PDF
        ├── schematic_diagram.svg       # Full Schematic in Vector SVG
        ├── pcb_layout_top.svg          # 2D Top Layer Vector Plot
        └── pcb_layout_bottom.svg       # 2D Bottom Layer Vector Plot
```
