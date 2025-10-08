# HBG3
https://rtr.ca/hbg3/
I've made this alternative location for documentation and files for the Celestron Homebrew Gadget 3 by Mark Lord. This will be removed if an official GitHub becomes available.

[Version history on Mark's website](https://rtr.ca/hbg3/history.html)

[Check the Wiki for an evolving guide](https://github.com/Mraanderson/HBG3/wiki)

[CloudyNights HBG3 thread](https://www.cloudynights.com/topic/854327-homebrew-gen3-pcb-wifibtgpsmusbrelay)

[CloudyNights dew control thread](https://www.cloudynights.com/topic/848895-homebrew-celestron-compatible-dew-heater-control)


[BIN file for direct flashing](https://rtr.ca/hbg3/flashing/hbg3.bin)
This link will always download the latest binary file direct from MLORD - the creator. It is the fastest and less fuss method for getting the code onto an ESP32. See the WiKi for full step by step.

----

# HBG3 — HomeBrew Gen3 WiFi Interface for Celestron Telescopes

A DIY ESP32-based WiFi/Bluetooth/GPS interface for Celestron telescope mounts. Supports CPWI, SkyPortal, SkySafari+, and more — no hand controller required.

---

## 🚀 Features

- WiFi and Bluetooth control of Celestron mounts  
- Compatible with CPWI, SkyPortal, SkySafari+  
- Optional GPS module for automatic location/time sync  
- USB-Serial and Bluetooth-Serial support  
- Relay mode for StarSense AutoAlign on older mounts  
- Fully open-source and DIY-friendly  

---

## 🧲 Supported Mounts

- NexStar SE / SLT / Evolution  
- CGEM / CGX / AVX  
- NexStar GPS (2000s)  
- Any Celestron mount with AUX ports  

---

## 🔧 Getting Started

1. Build the circuit using ESP32 DevKit-V1 and optional GPS  
2. Flash the firmware from this repo  
3. Connect to AUX port on your mount  
4. Use CPWI or SkyPortal to connect via WiFi  

See the [Build Guide](https://github.com/Mraanderson/HBG3/wiki/Build-Guide) for step-by-step instructions.

---

## 📚 Resources

- [Cloudy Nights HBG3 Discussion](https://www.cloudynights.com/topic/892198-hbg3-with-astronomy-sw-kstars-indi-others/)  
- [HBG3 PCB Build Guide](https://www.cloudynights.com/topic/854327-homebrew-gen3-pcb-wifibtgpsmusbrelay/)  
- [HBG3 AiO PCB Expansion](https://www.cloudynights.com/topic/975366-hbg3-aio-pcb/)  
- [CAN Site Reference](https://www.can.telescope.dev/hbg3)  

---

## 🙌 Credits

Created by mlord and the Cloudy Nights community.  
PCB designs, firmware, and schematics are open-source and shared for educational and hobbyist use.

---

## 📜 License

MIT License — see [LICENSE](LICENSE)
