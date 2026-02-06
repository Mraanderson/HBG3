# HBG3 Loader — Flashing & Build Guide (`flashinfo.md`)

This document explains how to build, flash, and maintain the HBG3 Loader firmware.  
It is designed so you (or anyone else) can return to this project later and immediately understand the workflow.

---

# 1. Project Overview

The HBG3 Loader is a minimal “foot‑in‑the‑door” firmware for ESP32 devices.  
Its purpose:

1. Boot  
2. Connect to WiFi (or start WiFiManager)  
3. Wait 2 seconds  
4. Auto‑update from a remote HTTPS URL  
5. Retry once if needed  
6. Reboot into the full firmware  

It requires no user interaction beyond WiFi setup.

---

# 2. Build Environment (PlatformIO)

PlatformIO is used instead of Arduino IDE because it reliably outputs:

- bootloader.bin  
- partitions.bin  
- boot_app0.bin (if required)  
- firmware.bin  

These are required for the web flasher.

### Folder structure

/hbg3_loader/
platformio.ini
partitions.csv
/src/
hbg3_loader_v1.ino

Code

### Required libraries

PlatformIO installs these automatically:

lib_deps =
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306
adafruit/Adafruit SH110X
tzapu/WiFiManager

Code

---

# 3. PlatformIO Configuration (`platformio.ini`)

Configuration for ESP32‑WROOM‑32:

[env:esp32_wroom_loader]
platform = espressif32
board = esp32dev
framework = arduino

board_build.flash_mode = dio
board_build.flash_size = 4MB
board_build.partitions = partitions.csv

upload_speed = 921600
monitor_speed = 115200

lib_deps =
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306
adafruit/Adafruit SH110X
tzapu/WiFiManager

Code

---

# 4. Partition Table (`partitions.csv`)

Name, Type, SubType, Offset, Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  ota_0,   0x10000, 0x140000,
app1,     app,  ota_1,   0x150000,0x140000,
spiffs,   data, spiffs,  0x290000,0x170000,

Code

This provides:

- Two OTA slots (1.25MB each)  
- Enough space for WiFiManager + HTTPS OTA  
- A SPIFFS region (unused but harmless)

---

# 5. Build Output Files

After building, PlatformIO generates:

| File             | Purpose          | Flash Offset |
|------------------|------------------|--------------|
| bootloader.bin   | Bootloader       | 0x1000       |
| partitions.bin   | Partition table  | 0x8000       |
| boot_app0.bin    | OTA helper       | 0xE000       |
| firmware.bin     | Application      | 0x10000      |

These are the files used by the web flasher.

---

# 6. Web Flasher Manifest (`manifest.json`)

Correct manifest for ESP32‑WROOM‑32:

{
"name": "HBG3 Loader (ESP32 WROOM)",
"version": "1.0.0",
"builds": [
{
"chipFamily": "ESP32",
"parts": [
{ "path": "bootloader.bin", "offset": 4096 },
{ "path": "partitions.bin", "offset": 32768 },
{ "path": "boot_app0.bin", "offset": 57344 },
{ "path": "firmware.bin", "offset": 65536 }
]
}
]
}

Code

### Manifest caching

GitHub Pages caches `manifest.json`.

Always bump the version:

manifest.json?v=2
manifest.json?v=3

Code

Then hard‑refresh the browser.

---

# 7. Loader Behaviour (Runtime Flow)

### Boot
- OLED: “HBG3 Loader / Booting…”
- LED off

### WiFi Attempt
- If saved WiFi exists → connect  
- If not → start WiFiManager AP  
  - SSID: `HBG3-Setup-XXXX`  
  - Portal at `http://192.168.4.1`

### 2‑Second Grace Period
- OLED: “Preparing update…”  
- Press BOOT to cancel auto‑update

### OTA Attempt #1
- LED fast blink  
- OLED shows progress  
- Downloads firmware from:  
  `https://rtr.ca/hbg3/firmware.bin`

### If fail → retry after 3 seconds

### If fail twice
- OLED: “Update failed / Press BOOT to retry”

### If success
- OLED: “Update complete / Rebooting…”

---

# 8. Web Console Output (Explained)

Example boot log:

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
HBG3 Foot-in-the-door Loader (auto OTA)
[WiFi] Starting WiFiManager config portal if needed
*wm:No wifi saved
*wm:StartAP with SSID: HBG3-Setup-DEA294
*wm:AP IP address: 192.168.4.1

Code

Meaning:

- Device booted normally  
- No WiFi saved  
- Setup AP started  
- User must connect to `HBG3-Setup-XXXX`

---

# 9. Recovery / Troubleshooting

### Device stuck rebooting
- Hold BOOT  
- Plug in USB  
- Release BOOT after 2 seconds  
- Flash again

### WiFiManager not appearing
- Forget the network on your phone  
- Reconnect to `HBG3-Setup-XXXX`  
- Open: `http://192.168.4.1`

### OTA fails repeatedly
- Try a mobile hotspot  
- Press BOOT to retry  
- Check firewall / DNS filtering (Pi‑hole, etc.)

---

# 10. Picking Up the Project Later

When returning to this project:

1. Open the PlatformIO folder  
2. Build → get `firmware.bin`, `bootloader.bin`, etc.  
3. Update `manifest.json`  
4. Bump the version (`?v=4`)  
5. Flash via the web flasher  
6. Watch the console for WiFiManager or OTA progress  

Everything else is automatic.

---
