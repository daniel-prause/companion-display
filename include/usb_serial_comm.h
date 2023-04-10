#pragma once
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include "USB.h"
#include <tuple>
#define USB_TIMEOUT 2000;
class USBSerialComm
{

public:
    USBSerialComm();
    ~USBSerialComm();
    void init();
    // first step
    int readCommand();
    uint_fast32_t readPayloadLength();
    // second step
    std::tuple<uint_fast32_t, char *> readPayload();
    bool connected();
    size_t read_raw(char *buffer, unsigned int len);

private:
    unsigned long lastPackageReceived = 0;
    size_t currentBufferLength = 0;
    char *buffer = (char *)malloc(320 * 170 * 3);
    USBCDC USBSerial;
};
