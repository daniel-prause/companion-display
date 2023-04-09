#include "usb_serial_comm.h"
#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

bool trigger = false;
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
unsigned long timeBegin = millis();
int totalBytes = 0;
USBSerialComm comm;
void setup()
{
  comm.init();
  // init tft
  tft.init();
  tft.setRotation(5);
  tft.setSwapBytes(true);
  tft.fillScreen(UINT32_MAX / 2);
  tft.setTextPadding(0);
  tft.setTextColor(0);
}

void loop()
{

  auto payload = comm.readPayload();
  if (std::get<0>(payload) > 0)
  {
    if (!trigger)
    {
      timeBegin = millis();
    }
    trigger = true;
    int readBytes = std::get<0>(payload);
    totalBytes += readBytes;
  }

  if ((millis() - timeBegin) >= 1000)
  {
    char str[80] = "";
    tft.fillRect(0, 0, 200, 20, UINT32_MAX);
    tft.drawString(str, 0, 0);
    sprintf(str, "Time: {%d} Bytes: {%d}", (millis() - timeBegin), totalBytes);
    tft.drawString(str, 0, 0);
    timeBegin = millis();
    totalBytes = 0;
    trigger = false;
  }
}