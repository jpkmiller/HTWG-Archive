#include <OneWire.h> 
#include <DallasTemperature.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Temp
#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Parola
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 2
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_RIGHT;
textPosition_t scrollAlign = PA_CENTER;
uint16_t scrollPause = 2000; // in milliseconds

#define  BUF_SIZE  4
char curMessage[BUF_SIZE] = { "" };

unsigned long time = millis();

void setup()
{
  Serial.begin(57600);
  Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop()
{
  int16_t temp = 0;
  if (millis() - time > 500)
  {
    sensors.requestTemperatures();
    time = millis();
  }
  temp = sensors.getTempCByIndex(0);
  // Serial.print(temp);
  
  if (P.displayAnimate())
  {
    strcpy(curMessage, String(temp).c_str());
    P.displayReset();
  }
}
