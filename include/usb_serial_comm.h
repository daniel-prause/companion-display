#pragma once
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include "USB.h"
#include <tuple>

class USBSerialComm
{

public:
    USBSerialComm();
    ~USBSerialComm();
    void init();
    bool connected();
    size_t read_raw(char *buffer, unsigned int len);

private:
    unsigned long lastPackageReceived = 0;
    size_t currentBufferLength = 0;
    char *buffer = (char *)malloc(320 * 170 * 3);
    USBCDC USBSerial;
};
