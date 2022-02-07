// code for server (receiver)
#include <SoftwareSerial.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 1
#define CS_PIN 10

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


SoftwareSerial Output(9, 8); // RX -> TX, TX -> RX

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

const uint8_t CMD_LENGTH = 10; // 8 + 2
byte Message[CMD_LENGTH];

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
  myDisplay.begin();
  myDisplay.setIntensity(15);
  myDisplay.displayClear();
  Serial.begin(9600);
  Output.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.readBytes(Message, 9);
    uint8_t crc_out = CRC8(Message, 9);
    // if crc_out is 0 then cmd was send successfully
    int i;
    if (crc_out == 0) {
      Output.println("Succesfully received cmd");
      Message[8] = 0x00;
      Output.println((char *) Message);
      Output.println("Start comparison");
      for (i = 0; i < 11; i++) {
        uint8_t success = 0;
        // Output.println((char *) cmds[i]);
        if (strcmp((char *) cmds[i], (char *)Message) == 0) {
          Output.println("Success!!");
          break;
        }
      }
      Output.println(String(i));
      if (i == 0) {
        // off
        myDisplay.setIntensity(15);
        myDisplay.displayClear();
      } else {
        myDisplay.print(String(i - 1));
      }
    }
  }
  
  delay(500);
}
