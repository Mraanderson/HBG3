# HBG3 Bench Test & User Guide

This guide provides the full documentation for building a "Simulated Mount" test rig using an ESP32. This allows users to familiarize themselves with the HomeBrew Generation 3 (HBG3) ecosystem, Mark Lord's Android apps, and the OTA update process before final assembly.

---

## 1. Hardware Setup (The Bench Rig)

To test the software, you only need an ESP32 DevKit, an OLED display, and two slide or rocker switches.

### Wiring Diagram
| Component | ESP32 Pin | Purpose |
| :--- | :--- | :--- |
| **OLED VCC** | **3V3** | Power (ensure 3.3V logic) |
| **OLED GND** | **GND** | Common Ground |
| **OLED SCL** | **GPIO 22** | I2C Clock |
| **OLED SDA** | **GPIO 21** | I2C Data |
| **Red switch (WIFI)**| **GPIO 13** | WiFi Mode Toggle / Reset |
| **Blue switch (MUSB)**| **GPIO 15**| USB Mode (Mount vs Serial) |

---

## 2. Installation: The Two-Stage Process

The HBG3 web flasher uses a "Minimal Loader" strategy to ensure you always have the latest firmware without hunting for files.

### Stage 1: The Web Flasher
1. Connect your ESP32 to your PC via USB data cable (charge only will fail to detect).
2. Open the [HBG3 Web Flasher](https://rtr.ca/hbg3/flash/) in Chrome or Edge using a Windows/Linux/Mac PC.
3. Click **Connect** and select your ESP32 port.
4. The flasher will install the **Minimal Loader**.

### Stage 2: The Full OTA Update
1. Once flashed, the ESP32 reboots and creates a WiFi Access Point: `HBG3-xxxx`.
2. Connect your phone/PC to this network and go to `192.168.4.1` in your browser.
3. Entering details to get an internet connection will trigger the **OTA Update**. 
4. The device will automatically download and install the full **Latest Release Production Firmware** direct from rtr.ca/hbg3.

---

## 3. Integrated Android Apps
Mark Lord has developed two companion apps to modernize the connection process. Testing these helps move them through Google Play's internal testing phase.

### **HomeBrew BT Bridge**
* **The Problem:** WiFi connections to telescopes can be unstable on Android because the phone realizes the "network" has no internet.
* **The Solution:** This app creates a **Bluetooth Bridge**.
* **Usage:** Connect the app to your HBG3 via Bluetooth. In SkySafari/SkyPortal, set the IP to `127.0.0.1`. The app handles the heavy lifting. TBC

### **Celestron WiFi Config**
* **The Problem:** Typing WiFi SSID and passwords into a terminal soon is difficult.
* **The Solution:** A native app to scan for the HBG3 and instantly configure its SSID/Password settings.

---

## 4. Key Bench Test Features

### **Mount Simulation**
By default, the HBG3 can act as a "Fake Mount."
* **Test:** Connect **CPWI (PC)** to the ESP32 via USB or WiFi. 
* **Verify:** If you can "Connect" and "Slew" in CPWI, the OLED should show changing RA/DEC coordinates. This proves the logic is working perfectly.

### **Fake GPS**
* **Test:** In the web interface (`192.168.4.1`), enable **Fake GPS**.
* **Verify:** The OLED will show an asterisk `*` and your chosen coordinates. This allows you to simulate star alignments from your desk.

### **Physical Interface (The Switches)
* **BOOT (ESP32 button:** * *Short Press:* Toggles through the OLED menu screen.
    * *Hold:* Forces the device to collect the OTA update when the menu item is selected.
* **WIFI Switch (GPIO 13):** * *Off:* AP Mode. Toggles through WiFi modes (AP Mode -> Client Mode -> WiFi Off).
    * *On:* Client Mode for local WiFi or connecting to Hotspot.
* **MUSB Switch (GPIO 15):** * *GND:* Mount-USB mode (For connecting to PC/CPWI via USB).
    * *Open:* Serial/Console mode (For debugging and flashing).

---

## 5. OLED Status Icons
* **`AP`**: Access Point mode (The HBG3 is a hotspot).
* **`STA`**: Station mode (The HBG3 is connected to your home WiFi).
* **`BT` / `BLE`**: A Bluetooth or BLE connection is active.
* **`*`**: A valid GPS lock (Real or Fake) is established.
  
