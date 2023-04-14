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
    WebPDecoderConfig config;
    WebPInitDecoderConfig(&config);
    config.options.use_threads = true;
    auto decoded_data = WebPDecode((const uint8_t *)image.c_str(), image.length(), &config);
    //  rgb888 to rgb565
    bool return_value = false;
    if (decoded_data == VP8_STATUS_OK)
    {
        for (int i = 0; i < width * height * 3; i += 3)
        {
            uint8_t r = config.output.u.RGBA.rgba[i];
            uint8_t g = config.output.u.RGBA.rgba[i + 1];
            uint8_t b = config.output.u.RGBA.rgba[i + 2];
            buffer[i / 3] = convert_and_combine(r, g, b);
        }
        return_value = true;
    }
    WebPFreeDecBuffer(&config.output);
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