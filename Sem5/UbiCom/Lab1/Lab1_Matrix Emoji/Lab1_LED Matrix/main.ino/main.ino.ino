#include <LedControl.h>

/*
pin 11 is connected to the DataIn
pin 13 is connected to the CLK ´
pin 10 is connected to LOAD 
We have only a single MAX72XX.
*/

LedControl lc=LedControl(11, 13, 10, 1);

byte smile1[8]={
  B00000000,
  B01100110,
  B01100110,
  B00000000,
  B00000000,
  B01000010,
  B00111100,
  B00000000
};

byte smile2[8]={
  B00000000,
  B01100110,
  B01100110,
  B00000000,
  B00111100,
  B00100100,
  B00111100,
  B00000000
};

void setup() {
   lc.shutdown(0,false);
   lc.setIntensity(0,8);
   lc.clearDisplay(0);
}

void smile(){
  int row, smile;
  for (smile = 1;;) {
    for (row = 0; row < 8; row++) {
      switch (smile) {
        case 0:
          lc.setRow(0, row, smile1[row]);
          break;
        case 1:
          lc.setRow(0, row, smile2[row]);
          break;
      }
    }
    delay(1000);
    lc.clearDisplay(0);
    smile = (smile + 1) % 2;
  }
}

void loop() { 
  smile();
}
