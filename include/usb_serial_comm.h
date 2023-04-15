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
    size_t write_raw(char *buffer, unsigned int len);

private:
    unsigned long lastPackageReceived = 0;
    USBCDC USBSerial;
};
