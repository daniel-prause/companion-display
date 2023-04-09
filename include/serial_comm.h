#pragma once
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include "USB.h"
#include <tuple>
class SerialComm
{
public:
    SerialComm();
    ~SerialComm();
    void init();
    // first step
    int readCommand();
    uint_fast32_t readPayloadLength();
    // second step
    std::tuple<uint_fast32_t, char *> readPayload();

private:
    size_t currentBufferLength = 0;
    char *buffer = (char *)malloc(320 * 170 * 3);
};
