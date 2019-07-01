# LoraDroneTracker
GPS tracker (not only)for drone using LoRa communication. 
Repository consists from 2 PlatformIO projects: `LoraDroneTracker_T` - for trasnceiver and `LoraDroneTracker_R` for receiver board.
Check [this figure](https://github.com/MatejBosansky/LoraDroneTracker/tree/master/Docs/LoraDroneTracker_vis.png) to get quick overview how it works and what all you need.


## Features
- No display is needed. This project uses serial communication with smartphone to get location data. You will recieve direct link for google maps.
- Unlike the GSM based GPS tracking devices, you don't need to use SIM card and you don't need to rely on signal of GSM operator.
- Possible to hibernate controler for specific ammount of time from beginning of flight - for example transciever starts to working after 17 minutes from start. 
- Basic and easy to understand project using PlatformIO. Good base point for your further modifications.

## Hardware
Additionally to soldering iron you will need:

### Transceiver
Device attached to drone.
- [ESP32 board](http://s.click.aliexpress.com/e/QBwIOJm) with LoRa Module supported by [this library](https://github.com/sandeepmistry/arduino-LoRa) +antenna. Every region is using [specific frequency for LoRa communication](http://www.3glteinfo.com/lora/lorawan-frequency-bands/), buy board with correct frequency band.
- GPS module with UART communication interface. I used [BN-180](http://s.click.aliexpress.com/e/bRQyfqUo).
- [Lipo battery](https://www.aliexpress.com/item/32853990762.html) with PicoBlade1 (1.25mm spacing) connector.
- Small basic switch for battery (not mandatory)

### Receiver
- [ESP32 board](http://s.click.aliexpress.com/e/QBwIOJm) with LoRa Module supported by [this library](https://github.com/sandeepmistry/arduino-LoRa) +antenna. Every region is using [specific frequency for LoRa communication](http://www.3glteinfo.com/lora/lorawan-frequency-bands/), buy board with correct frequency band.
- Smartphone - tested with Android

- Powerbank and installed [Serial Bluetooth Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en) app.
OR
- OTG cable and installed [Serial USB Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en) app.

## Notes
- For most of TTGO and their cloned boards you can charge LiPO battery through the board.
- Most of GPS modules are not able to lock position when are used indoor, you will need to test it outside.
- Lora signal strength very depends on antenna, quality of board and used LoRa communication settings. Check [some guides](https://medium.com/home-wireless/testing-lora-radios-with-the-limesdr-mini-part-2-37fa481217ff) how to configure LoRa communiction devices.
- For receiver you can use larger antenna to get better range results. With stock antennas I was able to receive not corrupted messages from 1km distance.
