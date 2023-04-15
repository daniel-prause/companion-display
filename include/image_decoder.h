#pragma once
#include <stdint.h>
#include <webp/demux.h>
#include <string>

class ImageDecoder
{
public:
    ImageDecoder();
    ~ImageDecoder();
    bool decode(std::string image, uint16_t *buffer, unsigned int width = 320, unsigned int height = 170);

private:
    std::string image;

    // combine two bytes
    inline unsigned short combine_bytes(uint8_t b1, uint8_t b2);

    // convert to 565
    inline unsigned short convert_and_combine(uint8_t r, uint8_t g, uint8_t b);
    // decode packet

    uint8_t closest_rb(uint8_t c);
    uint8_t closest_g(uint8_t c);
    uint16_t dither_xy(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    uint16_t RGB16BIT(uint8_t r, uint8_t g, uint8_t b);

    WebPDecoderConfig config;
};