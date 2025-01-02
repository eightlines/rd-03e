# Rd-03e 24GHz Gesture Recognition Sensor

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

## Issues
- Arduino Uno Software Serial supports a max baud of 57600. The Rd-30e ships with a factory setting of 256000 baud. So Software Serial UART cannot receive the data from the board. Need to install this on an ESP32 board with HardwareSerial support. 
- [VCP Driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers) for Adafruit HUZZAH32 Feather