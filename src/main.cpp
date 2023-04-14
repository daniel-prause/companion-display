#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "disconnected_image.h"
#include "usb_serial_comm.h"
#include "packetizer.h"
#include "image_decoder.h"

TFT_eSPI tft; // Invoke custom library
USBSerialComm comm;
Packetizer packetizer;
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

  // auto payload = comm.readPayload();
  char buffer[4096] = "";

  // append to packetizer
  packetizer.append_to_backlog(buffer, comm.read_raw(buffer, sizeof(buffer)));

  std::string next_packet = packetizer.next_packet();
  if (next_packet.length() != 0)
  {
    ImageDecoder image_decoder(next_packet);
    image_decoder.decode(last_image);
    tft.pushImage(0, 0, 320, 170, last_image);
  }

  if (!comm.connected())
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
    packetizer.clear_backlog();
  }
}