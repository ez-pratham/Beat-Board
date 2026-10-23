// ======================================================================================
//   ESP32 CYBER-CONSOLE MUSIC PAD - MULTI-MODE SIMULATOR (808 DRUMS & TRUMPET SYNTH)
// ======================================================================================
// Wiring:
//   - ILI9341 3.2" TFT Display:
//       CS=GPIO 5, D/C=GPIO 4, MOSI=GPIO 23, SCK=GPIO 18, VCC=3.3V, GND=GND, LED=3.3V
//   - Touch Pad Buttons (Active LOW):
//       PAD 1 -> GPIO 32 (Kick / Trumpet Valve 1: Bb3)
//       PAD 2 -> GPIO 33 (Snare / Trumpet Valve 2: D4)
//       PAD 3 -> GPIO 27 (Hi-Hat / Trumpet Valve 3: F4)
//       PAD 4 -> GPIO 14 (Clap / Trumpet Valve 4: Bb4)
//   - Master Control Pad (Active LOW):
//       SW1: UP     -> GPIO 34 (BPM+ in Drums / Octave+ in Trumpet / Menu Up)
//       SW2: DOWN   -> GPIO 35 (BPM- in Drums / Octave- in Trumpet / Menu Down)
//       SW3: SELECT -> GPIO 13 (Kit Toggle in Drums / Mute Toggle in Trumpet / Confirm in Menu)
//       SW4: MODE   -> GPIO 12 (Open / Toggle Mode Menu)
//   - Audio Output:
//       Buzzer / Audio Out -> GPIO 22
// ======================================================================================

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// --- Pin Definitions ---
#define TFT_CS           5
#define TFT_DC           4

#define BUZZER_PIN       22
#define TP1_PIN          32  // PAD 1
#define TP2_PIN          33  // PAD 2
#define TP3_PIN          27  // PAD 3
#define TP4_PIN          14  // PAD 4

#define BTN_UP_PIN       34  // SW1: UP
#define BTN_DOWN_PIN     35  // SW2: DOWN
#define BTN_SELECT_PIN   13  // SW3: SELECT
#define BTN_MODE_PIN     12  // SW4: MODE

// --- Color Palette (RGB565) ---
#define COLOR_BG         0x0821  // Cyber Slate
#define COLOR_HEADER     0x10A2  // Titanium Header
#define COLOR_CYAN       0x07FF  // Neon Cyan
#define COLOR_ORANGE     0xFD20  // Amber Orange
#define COLOR_GOLD       0xFFE0  // Gold / Brass Yellow
#define COLOR_MAGENTA    0xF81F  // Electric Magenta
#define COLOR_WHITE      0xFFFF
#define COLOR_BLACK      0x0000
#define COLOR_GREEN      0x07E0
#define COLOR_RED        0xF800
#define COLOR_GRAY_DIM   0x52AA
#define COLOR_BORDER_DIM 0x3186

// Display Object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// --- Multi-Mode & Menu State ---
enum InstrumentMode {
  MODE_DRUMS = 0,
  MODE_TRUMPET = 1
};
InstrumentMode currentMode = MODE_DRUMS;
bool isMenuOpen = false;
int menuSelection = 0; // 0: 808 DRUM MACHINE, 1: TRUMPET BRASS LEAD
const int TOTAL_MODES = 2;

// Sequencer & Tempo State (Drum Mode)
int bpm = 128;
int prevBpm = -1;
bool isLoopRunning = true;
bool prevLoopRunning = false;
int currentStep = 0;
int prevStep = -1;
unsigned long lastStepTime = 0;
int currentKit = 0; // 0: 808 Vintage, 1: Cyber Punch
int prevKit = -1;

// Trumpet & Brass State (Trumpet Mode)
int trumpetOctave = 0; // -1: Low, 0: Mid (Standard Bb), +1: High
int prevTrumpetOctave = -99;
bool isMutedHorn = false; // False: Open Horn, True: Harmon Mute
bool prevMutedHorn = true;
int activeTrumpetValve = -1;
unsigned long valveGlowTimer = 0;

const int TRUMPET_BASE_FREQ[4] = {233, 294, 349, 466};
const char* TRUMPET_NOTE_NAMES[4] = {"Bb3 (ROOT)", "D4 (3RD)", "F4 (5TH)", "Bb4 (OCTAVE)"};

// 16-Step Beat Pattern (Drums)
const bool patternKick[16]  = {1, 0, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0};
const bool patternSnare[16] = {0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0};
const bool patternHiHat[16] = {1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 0,  1, 0, 1, 1};
const bool patternClap[16]  = {0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 0, 0,  0, 0, 1, 0};

// Button Debounce & Edge Detection
unsigned long lastBtnCheck = 0;
bool lastUpState = HIGH, lastDownState = HIGH, lastSelectState = HIGH, lastModeState = HIGH;
bool lastTP1 = HIGH, lastTP2 = HIGH, lastTP3 = HIGH, lastTP4 = HIGH;

// Hit Glow Timers
unsigned long glowKick = 0, glowSnare = 0, glowHiHat = 0, glowClap = 0;
bool isKickLit = false, isSnareLit = false, isHiHatLit = false, isClapLit = false;

// Forward Declarations
void renderStaticDrumSet();
void renderTrumpetScreen();
void drawMenuScreen();
void updateHUD(bool forceAll = false);
void updateTrumpetHUD(bool forceAll = false);
void drawKick(bool hit);
void drawSnare(bool hit);
void drawHiHat(bool hit);
void drawClap(bool hit);
void drawBadge(int padIdx, bool hit);
void drawSequencerBar();
void drawSequencerStep(int step, bool active);
void drawTrumpetGraphic(int activeValve);

// --------------------------------------------------------------------------------------
// Audio Synthesizers (Buzzer / Audio Out)
// --------------------------------------------------------------------------------------
void playKickSound()  { Serial.println(">>> [PAD 1] KICK DRUM (808 Sub-Bass)"); tone(BUZZER_PIN, (currentKit == 0) ? 125 : 160, 45); }
void playSnareSound() { Serial.println(">>> [PAD 2] SNARE DRUM (Snap Tone)"); tone(BUZZER_PIN, (currentKit == 0) ? 360 : 440, 40); }
void playHiHatSound() { Serial.println(">>> [PAD 3] HI-HAT (Metallic Click)"); tone(BUZZER_PIN, 3200, 20); }
void playClapSound()  { Serial.println(">>> [PAD 4] HAND CLAP (Triple Burst)"); tone(BUZZER_PIN, 920, 30); }

void playTrumpetSound(int valveIdx) {
  float freq = TRUMPET_BASE_FREQ[valveIdx] * pow(2, trumpetOctave);
  Serial.printf(">>> [PAD %d] TRUMPET VALVE %d: %s (%.1f Hz) [%s]\n", 
                valveIdx + 1, valveIdx + 1, TRUMPET_NOTE_NAMES[valveIdx], freq, 
                isMutedHorn ? "HARMON MUTE" : "OPEN BRASS");
  
  if (isMutedHorn) {
    // Sharp brass bite
    tone(BUZZER_PIN, (int)(freq * 1.02), 20);
    delay(15);
    tone(BUZZER_PIN, (int)freq, 75);
  } else {
    // Open full trumpet blast with slight vibrato
    tone(BUZZER_PIN, (int)freq, 140);
  }
}

// --------------------------------------------------------------------------------------
// Triggers
// --------------------------------------------------------------------------------------
void triggerKick() {
  playKickSound();
  glowKick = millis() + 110;
  if (!isKickLit) { isKickLit = true; drawKick(true); drawBadge(0, true); }
}
void triggerSnare() {
  playSnareSound();
  glowSnare = millis() + 110;
  if (!isSnareLit) { isSnareLit = true; drawSnare(true); drawBadge(1, true); }
}
void triggerHiHat() {
  playHiHatSound();
  glowHiHat = millis() + 110;
  if (!isHiHatLit) { isHiHatLit = true; drawHiHat(true); drawBadge(2, true); }
}
void triggerClap() {
  playClapSound();
  glowClap = millis() + 110;
  if (!isClapLit) { isClapLit = true; drawClap(true); drawBadge(3, true); }
}

void triggerTrumpet(int valveIdx) {
  playTrumpetSound(valveIdx);
  activeTrumpetValve = valveIdx;
  valveGlowTimer = millis() + 160;
  drawTrumpetGraphic(valveIdx);
  drawBadge(valveIdx, true);
}

// --------------------------------------------------------------------------------------
// MODE SELECTION MENU OVERLAY
// --------------------------------------------------------------------------------------
void drawMenuScreen() {
  // Center Menu Popup Box
  tft.fillRoundRect(20, 24, 280, 192, 8, 0x0821);
  tft.drawRoundRect(20, 24, 280, 192, 8, COLOR_CYAN);
  tft.drawRoundRect(22, 26, 276, 188, 6, COLOR_BORDER_DIM);

  // Title Banner
  tft.fillRoundRect(26, 28, 268, 24, 4, COLOR_HEADER);
  tft.setTextColor(COLOR_CYAN, COLOR_HEADER);
  tft.setTextSize(1);
  tft.setCursor(65, 36);
  tft.print("--- SELECT INSTRUMENT MODE ---");

  const char* modeTitles[2] = {
    "[1] 808 DRUM MACHINE",
    "[2] TRUMPET / BRASS LEAD"
  };
  const char* modeDetails[2] = {
    "16-Step Sequencer | Kick, Snare, Hat, Clap",
    "4-Valve Lead Synth | Brass Harmonics & Tone"
  };

  for (int i = 0; i < TOTAL_MODES; i++) {
    int oy = 64 + i * 52;
    bool isSel = (menuSelection == i);

    if (isSel) {
      tft.fillRoundRect(32, oy, 256, 44, 5, 0x18C3);
      tft.drawRoundRect(32, oy, 256, 44, 5, COLOR_GOLD);
      tft.setTextColor(COLOR_GOLD, 0x18C3);
      tft.setTextSize(1);
      tft.setCursor(44, oy + 10);
      tft.printf("> %s  [ACTIVE]", modeTitles[i]);
      tft.setTextColor(COLOR_WHITE, 0x18C3);
      tft.setCursor(56, oy + 26);
      tft.print(modeDetails[i]);
    } else {
      tft.fillRoundRect(32, oy, 256, 44, 5, 0x10A2);
      tft.drawRoundRect(32, oy, 256, 44, 5, COLOR_BORDER_DIM);
      tft.setTextColor(COLOR_GRAY_DIM, 0x10A2);
      tft.setTextSize(1);
      tft.setCursor(44, oy + 10);
      tft.printf("  %s", modeTitles[i]);
      tft.setTextColor(COLOR_BORDER_DIM, 0x10A2);
      tft.setCursor(56, oy + 26);
      tft.print(modeDetails[i]);
    }
  }

  // Navigation Instructions Footer
  tft.fillRoundRect(26, 178, 268, 30, 4, COLOR_HEADER);
  tft.setTextColor(COLOR_GREEN, COLOR_HEADER);
  tft.setTextSize(1);
  tft.setCursor(38, 184);
  tft.print("UP/DOWN: Scroll | SELECT: Confirm");
  tft.setTextColor(COLOR_CYAN, COLOR_HEADER);
  tft.setCursor(38, 195);
  tft.print("MODE BUTTON: Close / Cancel Menu");
}

// --------------------------------------------------------------------------------------
// Drum UI Rendering
// --------------------------------------------------------------------------------------
void drawKick(bool hit) {
  const int cx = 115, cy = 168;
  if (hit) {
    tft.drawCircle(cx, cy, 44, COLOR_CYAN);
    tft.drawCircle(cx, cy, 42, COLOR_WHITE);
    tft.fillCircle(cx, cy, 39, COLOR_CYAN);
    tft.fillCircle(cx, cy, 16, COLOR_WHITE);
    tft.setTextColor(COLOR_BLACK, COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(cx - 24, cy - 4);
    tft.print("KICK 808");
  } else {
    tft.drawCircle(cx, cy, 44, COLOR_BG);
    tft.drawCircle(cx, cy, 42, COLOR_BG);
    tft.drawLine(cx - 32, cy + 24, cx - 48, cy + 42, COLOR_GRAY_DIM);
    tft.drawLine(cx + 32, cy + 24, cx + 48, cy + 42, COLOR_GRAY_DIM);
    tft.fillCircle(cx - 48, cy + 42, 2, 0x7BEF);
    tft.fillCircle(cx + 48, cy + 42, 2, 0x7BEF);

    tft.fillCircle(cx, cy, 39, COLOR_BG);
    tft.drawCircle(cx, cy, 39, 0x03EF);
    tft.drawCircle(cx, cy, 37, 0x2124);
    tft.fillCircle(cx, cy, 14, 0x18C3);
    tft.drawCircle(cx, cy, 14, 0x3186);

    tft.setTextColor(COLOR_CYAN, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(cx - 24, cy - 4);
    tft.print("KICK 808");
    tft.setTextColor(COLOR_GRAY_DIM, COLOR_BG);
    tft.setCursor(cx - 15, cy + 14);
    tft.print("PAD 1");
  }
}

void drawSnare(bool hit) {
  const int cx = 68, cy = 88;
  if (hit) {
    tft.drawCircle(cx, cy, 36, COLOR_ORANGE);
    tft.drawCircle(cx, cy, 34, COLOR_WHITE);
    tft.fillCircle(cx, cy, 31, COLOR_ORANGE);
    tft.fillCircle(cx, cy, 13, COLOR_WHITE);
    tft.setTextColor(COLOR_BLACK, COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(cx - 15, cy - 4);
    tft.print("SNARE");
  } else {
    tft.drawCircle(cx, cy, 36, COLOR_BG);
    tft.drawCircle(cx, cy, 34, COLOR_BG);
    tft.drawLine(cx, cy + 30, cx, cy + 45, COLOR_GRAY_DIM);
    tft.drawLine(cx, cy + 45, cx - 18, cy + 56, COLOR_GRAY_DIM);
    tft.drawLine(cx, cy + 45, cx + 18, cy + 56, COLOR_GRAY_DIM);

    tft.fillCircle(cx, cy, 31, 0x10A2);
    tft.drawCircle(cx, cy, 31, 0xCE60);
    tft.drawCircle(cx, cy, 29, 0x632C);
    tft.drawCircle(cx, cy, 12, 0x3186);

    tft.setTextColor(COLOR_ORANGE, 0x10A2);
    tft.setTextSize(1);
    tft.setCursor(cx - 15, cy - 4);
    tft.print("SNARE");
    tft.setTextColor(COLOR_GRAY_DIM, 0x10A2);
    tft.setCursor(cx - 14, cy + 12);
    tft.print("PAD 2");
  }
}

void drawHiHat(bool hit) {
  const int cx = 250, cy = 76;
  if (hit) {
    tft.fillRoundRect(cx - 30, cy - 5, 60, 13, 5, COLOR_GOLD);
    tft.fillCircle(cx, cy - 7, 7, COLOR_WHITE);
    tft.setTextColor(COLOR_BLACK, COLOR_GOLD);
    tft.setTextSize(1);
    tft.setCursor(cx - 18, cy + 18);
    tft.print("HI-HAT");
  } else {
    tft.drawLine(cx, 50, cx, 135, 0x632C);
    tft.fillRect(cx - 2, 54, 5, 5, 0x9CD3);
    tft.fillRoundRect(cx - 30, cy - 5, 60, 13, 5, 0x6B40);
    tft.drawRoundRect(cx - 30, cy - 5, 60, 13, 5, 0xDE80);
    tft.fillCircle(cx, cy - 7, 6, 0xDE80);
    tft.drawCircle(cx, cy - 7, 6, COLOR_GOLD);

    tft.setTextColor(COLOR_GOLD, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(cx - 18, cy + 18);
    tft.print("HI-HAT");
    tft.setTextColor(COLOR_GRAY_DIM, COLOR_BG);
    tft.setCursor(cx - 15, cy + 30);
    tft.print("PAD 3");
  }
}

void drawClap(bool hit) {
  const int bx = 186, by = 142, bw = 68, bh = 54;
  if (hit) {
    tft.drawRoundRect(bx - 2, by - 2, bw + 4, bh + 4, 7, COLOR_MAGENTA);
    tft.fillRoundRect(bx + 3, by + 3, bw - 6, bh - 6, 4, COLOR_MAGENTA);
    tft.setTextColor(COLOR_BLACK, COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(bx + 22, by + 21);
    tft.print("CLAP");
  } else {
    tft.drawRoundRect(bx, by, bw, bh, 5, 0x780F);
    tft.fillRoundRect(bx + 3, by + 3, bw - 6, bh - 6, 4, 0x1804);
    tft.setTextColor(COLOR_MAGENTA, 0x1804);
    tft.setTextSize(1);
    tft.setCursor(bx + 22, by + 21);
    tft.print("CLAP");
    tft.setTextColor(COLOR_GRAY_DIM, 0x1804);
    tft.setCursor(bx + 20, by + 37);
    tft.print("PAD 4");
  }
}

// --------------------------------------------------------------------------------------
// Trumpet Screen UI Rendering
// --------------------------------------------------------------------------------------
void drawTrumpetGraphic(int activeValve) {
  const int tx = 80, ty = 110;

  // Tubing & Horn Bell
  tft.fillRect(tx - 35, ty - 40, 220, 110, COLOR_BG);

  // Main Leadpipe Tubes
  tft.drawFastHLine(tx - 25, ty, 105, COLOR_GOLD);
  tft.drawFastHLine(tx - 25, ty + 12, 105, COLOR_GOLD);
  tft.drawFastVLine(tx - 25, ty, 12, COLOR_GOLD);

  // Mouthpiece
  tft.fillRect(tx - 38, ty + 1, 13, 10, 0xCE60);
  tft.drawFastVLine(tx - 38, ty - 3, 18, COLOR_WHITE);

  // 4 Brass Valves
  const int vX[4] = {tx + 5, tx + 28, tx + 51, tx + 74};
  for (int i = 0; i < 4; i++) {
    int vx = vX[i];
    bool isHit = (activeValve == i);
    int dy = isHit ? 8 : 0;

    // Valve Casing
    tft.drawRect(vx - 7, ty - 18, 14, 48, 0xDE80);
    tft.fillRect(vx - 5, ty - 16, 10, 44, 0x4200);

    // Valve Stem & Cap
    uint16_t capCol = isHit ? COLOR_CYAN : COLOR_GOLD;
    tft.drawFastVLine(vx, ty - 18 + dy, -10, COLOR_WHITE);
    tft.fillRoundRect(vx - 8, ty - 32 + dy, 16, 8, 3, capCol);
    tft.drawRoundRect(vx - 8, ty - 32 + dy, 16, 8, 3, COLOR_WHITE);

    // Valve Index
    tft.setTextColor(COLOR_BLACK, capCol);
    tft.setTextSize(1);
    tft.setCursor(vx - 3, ty - 31 + dy);
    tft.print(i + 1);
  }

  // Bell Flare Expanding Outward
  tft.drawLine(tx + 80, ty, tx + 130, ty - 22, COLOR_GOLD);
  tft.drawLine(tx + 80, ty + 12, tx + 130, ty + 34, COLOR_GOLD);
  tft.drawLine(tx + 130, ty - 22, tx + 130, ty + 34, COLOR_WHITE);

  // Radiating Acoustic Brass Soundwaves
  if (activeValve >= 0) {
    tft.drawCircle(tx + 144, ty + 6, 12, COLOR_CYAN);
    tft.drawCircle(tx + 154, ty + 6, 22, COLOR_GOLD);
    tft.drawCircle(tx + 164, ty + 6, 32, COLOR_WHITE);
  }

  // Note Readout Banner
  tft.fillRoundRect(40, 182, 240, 26, 4, COLOR_HEADER);
  tft.drawRoundRect(40, 182, 240, 26, 4, (activeValve >= 0) ? COLOR_GOLD : COLOR_BORDER_DIM);
  tft.setTextSize(1);
  if (activeValve >= 0) {
    float freq = TRUMPET_BASE_FREQ[activeValve] * pow(2, trumpetOctave);
    tft.setTextColor(COLOR_GOLD, COLOR_HEADER);
    tft.setCursor(50, 191);
    tft.printf("LIVE: %s (%.0f Hz)", TRUMPET_NOTE_NAMES[activeValve], freq);
  } else {
    tft.setTextColor(COLOR_GRAY_DIM, COLOR_HEADER);
    tft.setCursor(65, 191);
    tft.print("TAP PADS 1-4 TO PLAY TRUMPET");
  }
}

// --------------------------------------------------------------------------------------
// Bottom PAD Badges
// --------------------------------------------------------------------------------------
void drawBadge(int padIdx, bool hit) {
  int bx = 8 + padIdx * 77, by = 222, bw = 71, bh = 14;
  const char* drumLabels[4]    = {"1: KICK", "2: SNARE", "3: HI-HAT", "4: CLAP"};
  const char* trumpetLabels[4] = {"1: Bb3", "2: D4", "3: F4", "4: Bb4"};
  uint16_t colors[4]           = {COLOR_CYAN, COLOR_ORANGE, COLOR_GOLD, COLOR_MAGENTA};

  const char* textToShow = (currentMode == MODE_DRUMS) ? drumLabels[padIdx] : trumpetLabels[padIdx];

  if (hit) {
    tft.fillRoundRect(bx, by, bw, bh, 3, colors[padIdx]);
    tft.setTextColor(COLOR_BLACK, colors[padIdx]);
    tft.setTextSize(1);
    tft.setCursor(bx + 11, by + 3);
    tft.print(textToShow);
  } else {
    tft.fillRoundRect(bx, by, bw, bh, 3, 0x1082);
    tft.drawRoundRect(bx, by, bw, bh, 3, COLOR_BORDER_DIM);
    tft.setTextColor(colors[padIdx], 0x1082);
    tft.setTextSize(1);
    tft.setCursor(bx + 11, by + 3);
    tft.print(textToShow);
  }
}

// --------------------------------------------------------------------------------------
// 16-Step Sequencer LED Bar
// --------------------------------------------------------------------------------------
void drawSequencerBar() {
  for (int i = 0; i < 16; i++) {
    drawSequencerStep(i, (i == currentStep));
  }
}

void drawSequencerStep(int step, bool active) {
  int bx = 8 + step * 19, by = 25, bw = 16, bh = 8;
  bool hasBeat = patternKick[step] || patternSnare[step] || patternHiHat[step] || patternClap[step];

  if (active) {
    tft.fillRoundRect(bx, by, bw, bh, 2, COLOR_WHITE);
    tft.drawRoundRect(bx, by, bw, bh, 2, COLOR_GREEN);
  } else {
    uint16_t fillCol = (step % 4 == 0) ? 0x2124 : 0x10A2;
    tft.fillRoundRect(bx, by, bw, bh, 2, fillCol);
    tft.drawRoundRect(bx, by, bw, bh, 2, COLOR_BORDER_DIM);

    if (hasBeat) {
      uint16_t dotCol = patternKick[step] ? COLOR_CYAN :
                        (patternSnare[step] ? COLOR_ORANGE :
                        (patternHiHat[step] ? COLOR_GOLD : COLOR_MAGENTA));
      tft.fillCircle(bx + bw / 2, by + bh / 2, 2, dotCol);
    }
  }
}

// --------------------------------------------------------------------------------------
// Top HUD Banners
// --------------------------------------------------------------------------------------
void updateHUD(bool forceAll) {
  if (forceAll) {
    tft.fillRect(0, 0, 320, 22, COLOR_HEADER);
    tft.drawFastHLine(0, 22, 320, COLOR_BORDER_DIM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_CYAN, COLOR_HEADER);
    tft.setCursor(8, 7);
    tft.print("808 DRUM KIT");
  }

  if (forceAll || bpm != prevBpm) {
    tft.fillRect(96, 7, 56, 10, COLOR_HEADER);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WHITE, COLOR_HEADER);
    tft.setCursor(96, 7);
    tft.printf("BPM:%3d", bpm);
    prevBpm = bpm;
  }

  if (forceAll || currentKit != prevKit) {
    tft.fillRect(160, 7, 94, 10, COLOR_HEADER);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GOLD, COLOR_HEADER);
    tft.setCursor(160, 7);
    tft.print((currentKit == 0) ? "KIT: 808 VIN" : "KIT: CYBER  ");
    prevKit = currentKit;
  }

  if (forceAll || isLoopRunning != prevLoopRunning) {
    tft.fillRect(264, 7, 48, 10, COLOR_HEADER);
    tft.setTextSize(1);
    tft.setCursor(264, 7);
    if (isLoopRunning) {
      tft.setTextColor(COLOR_GREEN, COLOR_HEADER);
      tft.print("[RUN] ");
    } else {
      tft.setTextColor(COLOR_RED, COLOR_HEADER);
      tft.print("[STOP]");
    }
    prevLoopRunning = isLoopRunning;
  }
}

void updateTrumpetHUD(bool forceAll) {
  if (forceAll) {
    tft.fillRect(0, 0, 320, 22, COLOR_HEADER);
    tft.drawFastHLine(0, 22, 320, COLOR_BORDER_DIM);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_GOLD, COLOR_HEADER);
    tft.setCursor(8, 7);
    tft.print("TRUMPET BRASS");
  }

  if (forceAll || trumpetOctave != prevTrumpetOctave) {
    tft.fillRect(110, 7, 75, 10, COLOR_HEADER);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WHITE, COLOR_HEADER);
    tft.setCursor(110, 7);
    const char* octStr = (trumpetOctave == -1) ? "OCT: LOW " : ((trumpetOctave == 1) ? "OCT: HIGH" : "OCT: MID ");
    tft.print(octStr);
    prevTrumpetOctave = trumpetOctave;
  }

  if (forceAll || isMutedHorn != prevMutedHorn) {
    tft.fillRect(200, 7, 110, 10, COLOR_HEADER);
    tft.setTextSize(1);
    tft.setCursor(200, 7);
    if (isMutedHorn) {
      tft.setTextColor(COLOR_ORANGE, COLOR_HEADER);
      tft.print("[HARMON MUTE]");
    } else {
      tft.setTextColor(COLOR_CYAN, COLOR_HEADER);
      tft.print("[OPEN BRASS] ");
    }
    prevMutedHorn = isMutedHorn;
  }
}

// --------------------------------------------------------------------------------------
// Full Screen Renderers
// --------------------------------------------------------------------------------------
void renderStaticDrumSet() {
  tft.fillScreen(COLOR_BG);
  updateHUD(true);
  drawSequencerBar();

  drawKick(false);
  drawSnare(false);
  drawHiHat(false);
  drawClap(false);

  for (int i = 0; i < 4; i++) {
    drawBadge(i, false);
  }
}

void renderTrumpetScreen() {
  tft.fillScreen(COLOR_BG);
  updateTrumpetHUD(true);
  drawTrumpetGraphic(-1);

  for (int i = 0; i < 4; i++) {
    drawBadge(i, false);
  }
}

// --------------------------------------------------------------------------------------
// Input Polling & Sequencer Loop
// --------------------------------------------------------------------------------------
void readInputs() {
  bool curTP1 = digitalRead(TP1_PIN);
  bool curTP2 = digitalRead(TP2_PIN);
  bool curTP3 = digitalRead(TP3_PIN);
  bool curTP4 = digitalRead(TP4_PIN);

  if (currentMode == MODE_DRUMS && !isMenuOpen) {
    if (curTP1 == LOW && lastTP1 == HIGH) triggerKick();
    if (curTP2 == LOW && lastTP2 == HIGH) triggerSnare();
    if (curTP3 == LOW && lastTP3 == HIGH) triggerHiHat();
    if (curTP4 == LOW && lastTP4 == HIGH) triggerClap();
  } else if (currentMode == MODE_TRUMPET && !isMenuOpen) {
    if (curTP1 == LOW && lastTP1 == HIGH) triggerTrumpet(0);
    if (curTP2 == LOW && lastTP2 == HIGH) triggerTrumpet(1);
    if (curTP3 == LOW && lastTP3 == HIGH) triggerTrumpet(2);
    if (curTP4 == LOW && lastTP4 == HIGH) triggerTrumpet(3);
  }

  lastTP1 = curTP1; lastTP2 = curTP2; lastTP3 = curTP3; lastTP4 = curTP4;

  if (millis() - lastBtnCheck > 40) {
    lastBtnCheck = millis();
    bool curUp     = digitalRead(BTN_UP_PIN);
    bool curDown   = digitalRead(BTN_DOWN_PIN);
    bool curSelect = digitalRead(BTN_SELECT_PIN);
    bool curMode   = digitalRead(BTN_MODE_PIN);

    // MODE BUTTON: Opens / Closes Instrument Selection Menu
    if (curMode == LOW && lastModeState == HIGH) {
      isMenuOpen = !isMenuOpen;
      if (isMenuOpen) {
        menuSelection = (int)currentMode;
        Serial.println("[MODE] Opening Mode Menu...");
        drawMenuScreen();
      } else {
        Serial.println("[MODE] Exiting Menu...");
        if (currentMode == MODE_DRUMS) renderStaticDrumSet();
        else renderTrumpetScreen();
      }
    }

    if (isMenuOpen) {
      // Menu Navigation
      if (curUp == LOW && lastUpState == HIGH) {
        menuSelection = (menuSelection - 1 + TOTAL_MODES) % TOTAL_MODES;
        Serial.printf("[MENU UP] Highlight: %d\n", menuSelection);
        drawMenuScreen();
      }
      if (curDown == LOW && lastDownState == HIGH) {
        menuSelection = (menuSelection + 1) % TOTAL_MODES;
        Serial.printf("[MENU DOWN] Highlight: %d\n", menuSelection);
        drawMenuScreen();
      }
      if (curSelect == LOW && lastSelectState == HIGH) {
        currentMode = (InstrumentMode)menuSelection;
        isMenuOpen = false;
        Serial.printf("[MENU SELECT] Confirmed Mode: %s\n", (currentMode == MODE_DRUMS) ? "808 DRUMS" : "TRUMPET BRASS");
        if (currentMode == MODE_DRUMS) renderStaticDrumSet();
        else renderTrumpetScreen();
      }
    } else {
      // Normal Instrument Playing Controls
      if (currentMode == MODE_DRUMS) {
        if (curUp == LOW && lastUpState == HIGH) {
          bpm = min(240, bpm + 5);
          Serial.printf("[UP] BPM: %d\n", bpm);
          updateHUD();
        }
        if (curDown == LOW && lastDownState == HIGH) {
          bpm = max(50, bpm - 5);
          Serial.printf("[DOWN] BPM: %d\n", bpm);
          updateHUD();
        }
        if (curSelect == LOW && lastSelectState == HIGH) {
          currentKit = (currentKit + 1) % 2;
          Serial.printf("[SELECT] Kit: %s\n", currentKit == 0 ? "808 VINTAGE" : "CYBER PUNCH");
          updateHUD();
        }
      } else {
        // Trumpet Controls
        if (curUp == LOW && lastUpState == HIGH) {
          trumpetOctave = min(1, trumpetOctave + 1);
          Serial.printf("[UP] Trumpet Octave+: %d\n", trumpetOctave);
          updateTrumpetHUD();
        }
        if (curDown == LOW && lastDownState == HIGH) {
          trumpetOctave = max(-1, trumpetOctave - 1);
          Serial.printf("[DOWN] Trumpet Octave-: %d\n", trumpetOctave);
          updateTrumpetHUD();
        }
        if (curSelect == LOW && lastSelectState == HIGH) {
          isMutedHorn = !isMutedHorn;
          Serial.printf("[SELECT] Trumpet Mute: %s\n", isMutedHorn ? "HARMON MUTE" : "OPEN BRASS");
          updateTrumpetHUD();
        }
      }
    }

    lastUpState = curUp; lastDownState = curDown; lastSelectState = curSelect; lastModeState = curMode;
  }
}

void updateSequencer() {
  if (currentMode != MODE_DRUMS || !isLoopRunning || isMenuOpen) return;

  unsigned long stepInterval = (60000 / bpm) / 4;
  if (millis() - lastStepTime >= stepInterval) {
    lastStepTime = millis();

    if (prevStep != currentStep) {
      drawSequencerStep(prevStep, false);
      drawSequencerStep(currentStep, true);
      prevStep = currentStep;
    }

    if (patternKick[currentStep])  triggerKick();
    if (patternSnare[currentStep]) triggerSnare();
    if (patternHiHat[currentStep]) triggerHiHat();
    if (patternClap[currentStep])  triggerClap();

    currentStep = (currentStep + 1) % 16;
  }
}

void updateGlowDecay() {
  unsigned long now = millis();
  if (currentMode == MODE_DRUMS) {
    if (isKickLit && now >= glowKick)   { isKickLit = false; drawKick(false); drawBadge(0, false); }
    if (isSnareLit && now >= glowSnare) { isSnareLit = false; drawSnare(false); drawBadge(1, false); }
    if (isHiHatLit && now >= glowHiHat) { isHiHatLit = false; drawHiHat(false); drawBadge(2, false); }
    if (isClapLit && now >= glowClap)   { isClapLit = false; drawClap(false); drawBadge(3, false); }
  } else {
    if (activeTrumpetValve >= 0 && now >= valveGlowTimer) {
      int prevV = activeTrumpetValve;
      activeTrumpetValve = -1;
      drawTrumpetGraphic(-1);
      drawBadge(prevV, false);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("=========================================================");
  Serial.println(" ESP32 Cyber-Console Music Pad v2.4 (Drums & Trumpet)    ");
  Serial.println("=========================================================");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TP1_PIN, INPUT_PULLUP);
  pinMode(TP2_PIN, INPUT_PULLUP);
  pinMode(TP3_PIN, INPUT_PULLUP);
  pinMode(TP4_PIN, INPUT_PULLUP);

  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
  pinMode(BTN_MODE_PIN, INPUT_PULLUP);

  Serial.println("[1/3] Initializing ILI9341 Color Display...");
  tft.begin();
  tft.setRotation(1); // 320x240 landscape

  Serial.println("[2/3] Rendering Initial Instrument UI...");
  renderStaticDrumSet();

  Serial.println("[3/3] Audio Startup Chime...");
  tone(BUZZER_PIN, 440, 70); delay(80);
  tone(BUZZER_PIN, 554, 70); delay(80);
  tone(BUZZER_PIN, 659, 120);

  Serial.println("Ready! Press MODE button to open the Instrument Menu.");
}

void loop() {
  readInputs();
  updateSequencer();
  updateGlowDecay();
}
