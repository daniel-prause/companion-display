#include "image_decoder.h"
#include <memory>

// bpp - 1 = 8bit, 2 = 16 bit, 3 = 24 bit
ImageDecoder::ImageDecoder(std::string image)
    : image(image)
{
}

ImageDecoder::~ImageDecoder()
{
}

bool ImageDecoder::decode(uint16_t *buffer, unsigned int width, unsigned int height)
{
    // decode packet
    WebPData webp_data;
    webp_data.size = image.length();
    webp_data.bytes = new uint8_t[webp_data.size];
    int decoded_width = 0;
    int decoded_height = 0;
    auto decoded_data = WebPDecodeRGB((const uint8_t *)image.c_str(), webp_data.size, &decoded_width, &decoded_height);
    // rgb888 to rgb565
    bool return_value = false;
    if (decoded_data != NULL)
    {
        int x = 0;
        for (int i = 0; i < width * height * 3; i += 3)
        {
            uint8_t r = decoded_data[i];
            uint8_t g = decoded_data[i + 1];
            uint8_t b = decoded_data[i + 2];
            buffer[x] = convert_and_combine(r, g, b);
            x++;
        }
        return_value = true;
        WebPFree(decoded_data);
    }
    delete webp_data.bytes;
    return return_value;
}

unsigned short ImageDecoder::combine_bytes(uint8_t b1, uint8_t b2)
{
    int combined = b1 << 8 | b2;
    return combined;
}

unsigned short ImageDecoder::convert_and_combine(uint8_t r, uint8_t g, uint8_t b)
{
    unsigned int x1, x2; // The container for resulting 2 bytes

    x1 = (r & 0xF8) | (g >> 5);        // Take 5 bits of Red component and 3 bits of G component
    x2 = ((g & 0x1C) << 3) | (b >> 3); // Take remaining 3 Bits of G component and 5 bits of Blue component

    return combine_bytes(x1, x2);
}