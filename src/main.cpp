#include "usb_serial_comm.h"
#include "disconnected_image.h"
#include <SPI.h>
#include "packetizer.h"
#include <TFT_eSPI.h> // Hardware-specific library

bool trigger = false;
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
unsigned long timeBegin = millis();
int totalBytes = 0;
USBSerialComm comm;
int fps = 0;
Packetizer packetizer = Packetizer();
unsigned int total_packets = 0;

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
    total_packets += 1;
  }
  if (total_bytes_read != old_total_bytes_read)
  {

    char str[80] = "";
    tft.fillRect(0, 0, 200, 20, UINT32_MAX);
    sprintf(str, "BYTES: {%d} Packets: {%d}", total_bytes_read, total_packets);
    tft.println(str);
    tft.setCursor(0, 0);
  }

  /*
  if (comm.connected())
  {
    if (std::get<0>(payload) > 0)
    {
      char str[80] = "";
      tft.fillRect(0, 0, 200, 20, UINT32_MAX);
      sprintf(str, "BYTES: {%d}", std::get<0>(payload));
      tft.println(str);
      tft.setCursor(0, 0);
    }
  }
  else
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
  }*/
}