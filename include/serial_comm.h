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
    bool connected();
    size_t read_raw(char *buffer, unsigned int len);
    size_t write_raw(char *buffer, unsigned int len);

private:
    unsigned long lastPackageReceived = 0;
};
