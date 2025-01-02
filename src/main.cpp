#include <Arduino.h>
#include <HardwareSerial.h>

#define RX_PIN 16
#define TX_PIN 17

HardwareSerial radarSerial(1);

void sendCommand(uint8_t *command, size_t length) {
  radarSerial.write(command, length);
}

void readFirmwareVersion() {
  uint8_t command[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
  sendCommand(command, sizeof(command));

  delay(100);

  bool versionReceived = false;
  while (radarSerial.available()) {
    if (radarSerial.read() == 0xFD && radarSerial.read() == 0xFC && radarSerial.read() == 0xFB && radarSerial.read() == 0xFA) {
      radarSerial.read(); // Skip byte 1
      radarSerial.read(); // Skip byte 2

      if (radarSerial.read() == 0x00 && radarSerial.read() == 0x01) { // ACK
        uint8_t major = radarSerial.read();
        uint8_t minor = radarSerial.read();
        uint8_t patch = radarSerial.read();

        Serial.printf("Firmware Version: %d.%d.%d\n", major, minor, patch);

        versionReceived = true;
        break;       
      }
    }
  }

  if (!versionReceived) {
    Serial.println("Failed to read firmware version");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Radar Firmware Version Reader");

  radarSerial.begin(256000, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(1000);
}

void loop() {
  readFirmwareVersion();
  delay(1000);
}
