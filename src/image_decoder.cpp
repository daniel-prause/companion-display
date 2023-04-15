#include "image_decoder.h"
#include <memory>

// bpp - 1 = 8bit, 2 = 16 bit, 3 = 24 bit
ImageDecoder::ImageDecoder()
{
    WebPInitDecoderConfig(&config);
    config.options.use_threads = true;
}

ImageDecoder::~ImageDecoder()
{
}

bool ImageDecoder::decode(std::string image, uint16_t *buffer, unsigned int width, unsigned int height)
{
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
            buffer[i / 3] = dither_xy(i / 3 % width, i / 3 / width, r, g, b);
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

/* Dither Tresshold for Red Channel */
static const uint8_t dither_tresshold_r[64] = {
    1, 7, 3, 5, 0, 8, 2, 6,
    7, 1, 5, 3, 8, 0, 6, 2,
    3, 5, 0, 8, 2, 6, 1, 7,
    5, 3, 8, 0, 6, 2, 7, 1,

    0, 8, 2, 6, 1, 7, 3, 5,
    8, 0, 6, 2, 7, 1, 5, 3,
    2, 6, 1, 7, 3, 5, 0, 8,
    6, 2, 7, 1, 5, 3, 8, 0};

/* Dither Tresshold for Green Channel */
static const uint8_t dither_tresshold_g[64] = {
    1, 3, 2, 2, 3, 1, 2, 2,
    2, 2, 0, 4, 2, 2, 4, 0,
    3, 1, 2, 2, 1, 3, 2, 2,
    2, 2, 4, 0, 2, 2, 0, 4,

    1, 3, 2, 2, 3, 1, 2, 2,
    2, 2, 0, 4, 2, 2, 4, 0,
    3, 1, 2, 2, 1, 3, 2, 2,
    2, 2, 4, 0, 2, 2, 0, 4};

/* Dither Tresshold for Blue Channel */
static const uint8_t dither_tresshold_b[64] = {
    5, 3, 8, 0, 6, 2, 7, 1,
    3, 5, 0, 8, 2, 6, 1, 7,
    8, 0, 6, 2, 7, 1, 5, 3,
    0, 8, 2, 6, 1, 7, 3, 5,

    6, 2, 7, 1, 5, 3, 8, 0,
    2, 6, 1, 7, 3, 5, 0, 8,
    7, 1, 5, 3, 8, 0, 6, 2,
    1, 7, 3, 5, 0, 8, 2, 6};

/* Get 16bit closest color */
uint8_t ImageDecoder::closest_rb(uint8_t c)
{
    return (c >> 3 << 3); /* red & blue */
}
uint8_t ImageDecoder::closest_g(uint8_t c)
{
    return (c >> 2 << 2); /* green */
}

/* RGB565 */
uint16_t ImageDecoder::RGB16BIT(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint16_t)((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

/* Dithering by individual subpixel */
uint16_t ImageDecoder::dither_xy(
    int x,
    int y,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    /* Get Tresshold Index */
    uint8_t tresshold_id = ((y & 7) << 3) + (x & 7);

    r = ImageDecoder::closest_rb(
        std::min(r + dither_tresshold_r[tresshold_id], 0xff));
    g = ImageDecoder::closest_g(
        std::min(g + dither_tresshold_g[tresshold_id], 0xff));
    b = ImageDecoder::closest_rb(
        std::min(b + dither_tresshold_b[tresshold_id], 0xff));
    return ImageDecoder::RGB16BIT(r, g, b);
}
