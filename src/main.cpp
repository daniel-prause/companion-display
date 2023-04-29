#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "disconnected_image.h"
#include "serial_comm.h"
#include "packetizer.h"
#include "image_decoder.h"

TFT_eSPI tft; // Invoke custom library
SerialComm comm;
Packetizer packetizer;
ImageDecoder image_decoder;
uint16_t *last_image = new uint16_t[320 * 170];

void setup()
{
  // init usb comm
  comm.init();
  // init tft
  tft.init();
  tft.setRotation(5);
  tft.setSwapBytes(true);
  tft.setTextPadding(0);
  tft.setTextColor(0);
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

  switch (command)
  {
  case 228:
  {
    if (!payload.empty())
    {
      image_decoder.decode(payload, last_image);
    }
    tft.pushImage(0, 0, 320, 170, last_image);
    break;
  }
  case 229:
  {
    tft.pushImage(0, 0, 320, 170, last_image);
    break;
  }
  case 18:
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, 0);
    break;
  case 19:
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, 255);
    break;
  }

  if (!comm.connected())
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
    packetizer.clear_backlog();
  }
}