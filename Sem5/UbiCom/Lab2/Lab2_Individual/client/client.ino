// code for client (sender)
#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial Input(3, 2); // RX -> TX, TX -> RX

byte CRC8(const uint8_t *data, uint8_t len) {
  const uint8_t polynome = 0xD5;
  uint8_t crc = 0x00;
  while (len--) {
    crc ^= *data++;
    for (uint8_t i = 8; i; i--) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ polynome;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

const int BUFFER_SIZE = 8;
byte cmdAsByte[BUFFER_SIZE + 2]; // +1 for CRC // +1 for \0
 
byte cmds[11][9] = {
  {0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x00}, // off
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00}, // 0
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00}, // 1
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x00}, // ...
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x31, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x30, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x00},
  {0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x31, 0x00},
};

void setup() {
  Serial.begin(9600); // send
  Input.begin(9600); // receive
}

void loop() {
  // check if sending is possible
    delay(1);
    // read from virt terminal
    if (Input.available() > 0) {
      // write bytes into buffer
      Input.println("Zu Befehl, Sir!");
      String cmdAsString = Input.readStringUntil('\n');
      cmdAsString.trim(); // is important
      Input.println(cmdAsString);

      // parse cmd
      if (cmdAsString == "off") {
        memcpy(cmdAsByte, cmds[0], 9);
      } else {
        uint8_t number = cmdAsString.substring(11, 12).toInt();
        memcpy(cmdAsByte, cmds[number + 1 /* because -1 is at index 0 */], 9);
      }

      uint8_t crc = CRC8(cmdAsByte, 8);
      Input.println(String(crc));
      cmdAsByte[8] = (char) crc;
      // Input.println(String(crc));
      Input.println((char *) cmdAsByte);
    }

    Serial.println((char *) cmdAsByte); // byte array to char array
}
