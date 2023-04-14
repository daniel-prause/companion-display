#include "usb_serial_comm.h"
#include "disconnected_image.h"
#include <SPI.h>
#include "packetizer.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <webp/demux.h>
#include "image_decoder.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
USBSerialComm comm;
Packetizer packetizer = Packetizer();

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

unsigned int total_bytes_read = 0;

uint16_t *last_image = new uint16_t[320 * 170];

void loop()
{

  // auto payload = comm.readPayload();
  char buffer[1024] = "";
  int old_total_bytes_read = total_bytes_read;
  unsigned int new_bytes_read = comm.read_raw(buffer, sizeof(buffer));
  total_bytes_read += new_bytes_read;

  // append to packetizer
  packetizer.append_to_backlog(buffer, new_bytes_read);

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