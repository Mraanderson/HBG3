# Web Flasher

Web flasher to prepare your ESP32 for the full firmware by OTA update - https://mraanderson.github.io/HBG3/

----

# HBG3
https://rtr.ca/hbg3/
I've made this alternative location for documentation and files for the Celestron Homebrew Generation 3 by Mark Lord. This will be removed if an official GitHub becomes available. From v8.56 uploading of the code to GitHub and the use in AI has been explicitly stated so the repo should now reflect this position. Please respect the developers wishes and use as intended.

[Version history on Mark's website](https://rtr.ca/hbg3/history.html)

[Check the Wiki for an evolving guide](https://github.com/Mraanderson/HBG3/wiki)

[CloudyNights HBG3 thread](https://www.cloudynights.com/topic/854327-homebrew-gen3-pcb-wifibtgpsmusbrelay)

[CloudyNights dew control thread](https://www.cloudynights.com/topic/848895-homebrew-celestron-compatible-dew-heater-control)


[BIN file for direct flashing](https://rtr.ca/hbg3/flashing/hbg3.bin)
This link will always download the latest binary file direct from MLORD - the creator.

## Android Apps
[Homebrew BT Bridge](https://play.google.com/store/apps/details?id=com.marklord.homebrewbtbridge)

[Celestron WiFi Config](https://play.google.com/store/apps/details?id=com.marklord.celestronwificonfig)

----

# HBG3 — HomeBrew Gen3 WiFi Interface for Celestron Telescopes

A DIY ESP32-based WiFi/Bluetooth/GPS interface for Celestron telescope mounts. Supports CPWI, SkyPortal, SkySafari+, and more — no hand controller required.

---

## 🧲 Supported Mounts

- NexStar SE / SLT / Evolution  
- CGEM / CGX / AVX  
- NexStar GPS (2000s)  
- Any Celestron mount with AUX ports  

---

## 🔧 Getting Started

1. Build the device (https://rtr.ca/hbg3/assembly)
2. Flash the firmware from this repo
3. Update to the official firmware
4. Connect to AUX port on your mount  
5. Use CPWI or SkyPortal to connect via WiFi or Bluetooth  

First, update the firmware on the HBG3, following the instructions from the project site at https://rtr.ca/hbg3/

Using an Android phone/tablet download and install:
 * the free SkyPortal app https://play.google.com/store/apps/details?id=com.celestron.skyportal and configure the app to use "Access Point" mode for "connection" (somewhere in Settings)
 * the free HomeBrew BT Bridge app https://play.google.com/store/apps/details?id=com.marklord.homebrewbtbridge

Power on the mount with the HBG3 already plugged into it over AUX, and run the HomeBrew BT Bridge app.

Tap on your HomeBrew-xxxxxx device from the list it presents.  The run SkyPortal, and hit "CONNECT and ALIGN".

---

## Building ##

See the [Build Guide](https://rtr.ca/hbg3/assembly/) for step-by-step instructions.

---

## 📚 Resources

- [Cloudy Nights HBG3 Discussion](https://www.cloudynights.com/topic/892198-hbg3-with-astronomy-sw-kstars-indi-others/)  
- [HBG3 PCB Build Guide](https://rtr.ca/hbg3/assembly/)  
- [HBG3 AiO PCB Expansion](https://www.cloudynights.com/topic/975366-hbg3-aio-pcb/)  
- [CAN Site Reference](https://www.can.telescope.dev/hbg3)  

---

## 🙌 Credits

HBG3/HBG4 Created by mlord and supported on the Cloudy Nights community.  
PCB designs, firmware, and schematics are open-source and shared for educational and hobbyist use. NOT FOR RESALE.

---

## 📜 License

MIT License — see [LICENSE](LICENSE)
