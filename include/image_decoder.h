#pragma once
#include <stdint.h>
#include <PNGdec.h>
class ImageDecoder
{
public:
    ImageDecoder();
    ~ImageDecoder();
    void decode(char *buffer, size_t length);

private:
    uint8_t *buffer = 0;
    uint32_t current_image_length = 0;
    PNG png;
};