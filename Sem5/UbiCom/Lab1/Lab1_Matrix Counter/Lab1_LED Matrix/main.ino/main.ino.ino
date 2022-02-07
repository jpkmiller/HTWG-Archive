// Including the required Arduino libraries
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 3
#define CS_PIN 10

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  // Intialize the object
  myDisplay.begin();

  // Set the intensity (brightness) of the display (0-15)
  myDisplay.setIntensity(15);

  // Clear the display
  myDisplay.displayClear();
}

void counter() {
  int c1, c2, c3, c;
  c = 0;
  myDisplay.setTextAlignment(PA_CENTER);
  for (c1 = 0; c1 < 10; c1++) {
      for (c2 = 0; c2 < 10; c2++) {
          for (c3 = 0; c3 < 10; c3++) {
             c++;
             myDisplay.print(String(c1) + " " + String(c2) + " " + String(c3));
             delay(150);
             if (c == 128) {
                return;
             }
          }
      }
  }
}

void loop() {
  counter();
  delay(500);
}
