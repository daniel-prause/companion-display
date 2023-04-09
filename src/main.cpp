#include "usb_serial_comm.h"
#include "disconnected_image.h"
#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

bool trigger = false;
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
unsigned long timeBegin = millis();
int totalBytes = 0;
USBSerialComm comm;
int fps = 0;

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

  if (comm.connected())
  {
    if (std::get<0>(payload) > 0)
    {
      fps += 1;
      if (!trigger)
      {
        trigger = true;
        timeBegin = millis() + 1000;
      }
      if (timeBegin < millis())
      {
        char str[80] = "";
        tft.fillRect(0, 0, 200, 20, UINT32_MAX);
        sprintf(str, "FPS: {%d}", fps);
        tft.println(str);
        tft.setCursor(0, 0);
        timeBegin = millis() + 1000;
        fps = 0;
      }
    }
  }
  else
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
  }
}