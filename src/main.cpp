#include <Arduino.h>
#include <HardwareSerial.h>

#define RX_PIN 16
#define TX_PIN 17

HardwareSerial radarSerial(1);

bool radarReady = false;
bool targetInRange = false;
uint8_t buff[64] = {0};
uint8_t buffIndex = 0;
uint16_t range;

void sendCommand(uint8_t *command, size_t length) {
  radarSerial.write(command, length);
}

void readFirmwareVersion() {
  uint8_t firmwareCommand[] = {
    0xFD, 0xFC, 0xFB, 0xFA, // Frame Header
    0x02, 0x00, // Data Length (2 bytes follow)
    0x00, 0x00, // Command Word
    0x04, 0x03, 0x02, 0x01 // Frame Footer
  };

  sendCommand(firmwareCommand, sizeof(firmwareCommand));
  delay(100);

  bool versionReceived = false;

  while (radarSerial.available()) {
    if (radarSerial.read() == 0xFD 
     && radarSerial.read() == 0xFC 
     && radarSerial.read() == 0xFB 
     && radarSerial.read() == 0xFA) {

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

void setDistanceCalibration(int32_t calibrationValue) {
  uint8_t enableCommand[] = {
    0xFD, 0xFC, 0xFB, 0xFA, // Frame Header
    0x04, 0x00, // Data Length (4 bytes follow)
    0xFF, 0x00, // Command Word for Enable Configuration
    0x01, 0x00, // End of Frame
    0x04, 0x03, 0x02, 0x01 // Frame Footer
  };
  
  sendCommand(enableCommand, sizeof(enableCommand));
  delay(100);

  Serial.print("Enabled Configurtion ACK:");
  bool enableAckReceived = false;

  while (radarSerial.available()) {
    uint8_t byte = radarSerial.read();

    if (byte == 0xFD && radarSerial.read() == 0xFC) {
      enableAckReceived = true;
      Serial.printf("0x%02X\n", byte);
    } else if (enableAckReceived) {
      Serial.printf("0x%02X\n", byte);
      if (byte == 0x01) break;
    }
  }
  Serial.println();

  if (!enableAckReceived) {
    Serial.println("Enable Configuration ACK not received");
    return;
  }

  uint8_t calibrationCommand[] = {
    0xFD, 0xFC, 0xFB, 0XFA, // Frame Header
    0x08, 0x00, // Data Length (8 bytes follow)
    0x72, 0x00, // Command Word for Distance Calibration
    0x00, 0x00, // Distance Calibration Parameter Number
    (uint8_t)(calibrationValue & 0xFF), // Calibration Value byte 1
    (uint8_t)((calibrationValue >> 8) & 0xFF), // Calibration Value byte 2
    (uint8_t)((calibrationValue >> 16) & 0xFF), // Calibration Value byte 3
    (uint8_t)((calibrationValue >> 24) & 0xFF), // Calibration Value byte 4
    0x04, 0x03, 0x02, 0x01 // Frame Footer
  };

  sendCommand(calibrationCommand, sizeof(calibrationCommand));
  delay(100);

  Serial.print("Distance Calibration ACK:");
  bool calibrationAckReceived = false;

  while (radarSerial.available()) {
    uint8_t byte = radarSerial.read();
    Serial.printf("0x%02X\n", byte);
    
    if (byte == 0x01) {
      calibrationAckReceived = true;
      break;
    }
  }

  Serial.println();

  if (calibrationAckReceived) {
    Serial.println("Distance Calibration Command Successful");
  } else {
    Serial.println("Distance Calibration Command Failed");
  }

  uint8_t disableCommand[] = {
    0xFD, 0xFC, 0xFB, 0xFA, // Frame Header
    0x02, 0x00, // Data Length (2 bytes follow)
    0xFE, 0x00, // Command Word for Disable Configuration
    0x04, 0x03, 0x02, 0x01 // Frame Footer
  };

  sendCommand(disableCommand, sizeof(disableCommand));
  delay(100);

  Serial.print("Calibration Ended");
}

void checkRadar() {
  while (radarSerial.available()) {
    uint8_t byte = radarSerial.read();
    buff[buffIndex++] = byte;

    if (buffIndex >= 5) { // Min 5 bytes needed
      if (buff[0] == 0xAA
       && buff[buffIndex - 1] == 0x55) { // Extract Distance & Gesture

        range = (buff[2] << 8) | buff[1]; // Distance bytes
        uint8_t gesture = buff[3]; // Gesture byte
        float distInMeters = range / 100.0; // cm > m
       
        Serial.printf("Distance: %.2f m, Gesture: %d, Micro Motion: %d\n", distInMeters, gesture);

        if (gesture == 0x01) {
          Serial.println("Gesture");
        }

        memset(buff, 0, sizeof(buff));
        buffIndex = 0;
      } else {
        if (buffIndex >= sizeof(buff)) {
          memset(buff, 0, sizeof(buff));
          buffIndex = 0;
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) ;

  Serial.println("Radar Firmware Version Reader");

  radarSerial.begin(256000, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(1000);

  readFirmwareVersion();
  delay(1000);

  setDistanceCalibration(100);
  delay(4000);

  radarReady = true;
}

void loop() {
  if (radarReady) checkRadar();
}
