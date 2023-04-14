#include "usb_serial_comm.h"
#include "disconnected_image.h"
#include <SPI.h>
#include "packetizer.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <webp/demux.h>

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
int width = 0;
int height = 0;
int memory_size = 0;

// combine two bytes
unsigned short combine_bytes(byte b1, byte b2)
{
  int combined = b1 << 8 | b2;
  return combined;
}

// convert to 565
unsigned short convert_and_combine(uint8_t r, uint8_t g, uint8_t b)
{
  unsigned int x1, x2; // The container for resulting 2 bytes

  x1 = (r & 0xF8) | (g >> 5);        // Take 5 bits of Red component and 3 bits of G component
  x2 = ((g & 0x1C) << 3) | (b >> 3); // Take remaining 3 Bits of G component and 5 bits of Blue component

  return combine_bytes(x1, x2);
}
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
    // decode packet
    WebPData webp_data;
    webp_data.size = next_packet.length();
    webp_data.bytes = new uint8_t[webp_data.size];
    auto decoded_data = WebPDecodeRGB((const uint8_t *)next_packet.c_str(), webp_data.size, &width, &height);
    // rgb888 to rgb565
    if (decoded_data != NULL)
    {
      int x = 0;
      for (int i = 0; i < 320 * 170 * 3; i += 3)
      {
        uint8_t r = decoded_data[i];
        uint8_t g = decoded_data[i + 1];
        uint8_t b = decoded_data[i + 2];
        last_image[x] = convert_and_combine(r, g, b);
        x++;
      }

      tft.pushImage(0, 0, 320, 170, last_image);
    }

    WebPFree(decoded_data);
    delete webp_data.bytes;
  }
  if (!comm.connected())
  {
    tft.pushImage(0, 0, 320, 170, (const uint16_t *)&disconnected_image);
  }
}