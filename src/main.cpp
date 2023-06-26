#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <EEPROM.h>
#include "disconnected_image.h"
#include "serial_comm.h"
#include "packetizer.h"
#include "image_decoder.h"

TFT_eSPI tft; // Invoke custom library
SerialComm comm;
Packetizer packetizer;
ImageDecoder image_decoder;
uint16_t *last_image = new uint16_t[320 * 170]{};
const char EEPROM_BRIGHTNESS_POS = 0;
auto current_eeprom_brightness = 0;

void setup()
{
  // init usb comm
  comm.init();
  // init tft
  delay(100);
  tft.init();
  delay(1000);
  tft.setRotation(5);
  tft.setSwapBytes(true);
  tft.setTextPadding(0);
  tft.setTextColor(0);
  current_eeprom_brightness = EEPROM.read(EEPROM_BRIGHTNESS_POS);
  if (current_eeprom_brightness == 0)
  {
    current_eeprom_brightness = 255;
  }
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, current_eeprom_brightness);
}

void loop()
{

  char buffer[4096] = "";

  // append raw data to packetizer
  size_t len = comm.read_raw(buffer, sizeof(buffer));
  packetizer.append_to_backlog(buffer, len);

  // try to build next packet
  auto next_packet = packetizer.next_packet();
  auto command = std::get<0>(next_packet);
  auto payload = std::get<1>(next_packet);
  bool sleeping = false;

  switch (command)
  {
  case 228: // set new image
  {
    if (!payload.empty())
    {
      image_decoder.decode(payload, last_image);
    }
    tft.pushImage(0, 0, 320, 170, last_image);
    break;
  }
  case 229: // re-use last image
  {
    tft.pushImage(0, 0, 320, 170, last_image);
    break;
  }
  case 18: // sleep == set brightness to zero
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, 0);
    sleeping = true;
    break;
  case 19: // wake up == set brightness to 255
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, current_eeprom_brightness);
    sleeping = false;
    break;
  case 20: // set brightness to payload
    auto brightness = payload[0];
    if (brightness != current_eeprom_brightness)
    {
      EEPROM.write(EEPROM_BRIGHTNESS_POS, brightness);
      EEPROM.commit(); // debounce me!
      current_eeprom_brightness = brightness;
    }
    if (!sleeping)
    {
      ledcSetup(0, 10000, 8);
      ledcAttachPin(38, 0);
      ledcWrite(0, brightness);
    }
    break;
  }

  // show disconnected image
  if (!comm.connected())
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
    packetizer.clear_backlog();
  }

  // auto sleep mode
  if ((comm.last_package_received() + 60000 < millis()) && !sleeping)
  {
    sleeping = true;
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, 0);
  }
}