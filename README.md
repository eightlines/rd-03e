# Rd-03e 24GHz Gesture Recognition Sensor

- Detects distance of a single person (no X/Y location)
- Detects gesture (Only receiving 0x01 indication of gesture)
- 24 GHz K Band
- 6m movement max distance
- +/- 20deg horizontally
- +/- 45 deg vertically
- 5 VDC 200mA
- UART 256000 baud
- Installed 
	- 1.3m above floor
	- +ve angle Pinout Right when face on
	- -ve angle Left side from pinout
	- Up

## Pinout
- VCC - Blue - 5VDC
- GND - Green
- OT1 - Yellow - UART TX > D16
- RX - Orange - UART RX > D17
- OT2 - Red - Reserved

[RD-03e Spec Sheet](https://docs.ai-thinker.com/_media/rd-03e_v1.0.0_specification_20240103.pdf)
[RD-03e Ranging Manual](https://docs.ai-thinker.com/_media/rd-03e%E7%B2%BE%E5%87%86%E6%B5%8B%E8%B7%9D%E7%94%A8%E6%88%B7%E6%89%8B%E5%86%8C%E8%8B%B1%E6%96%87%E7%89%88.pdf)

## Issues
- Arduino Uno Software Serial supports a max baud of 57600. The Rd-30e ships with a factory setting of 256000 baud. So Software Serial UART cannot receive the data from the board. Need to install this on an ESP32 board with HardwareSerial support. 
- [VCP Driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers) for Adafruit HUZZAH32 Feather