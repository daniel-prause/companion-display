#include "serial_comm.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <algorithm>
constexpr unsigned int USB_TIMEOUT = 1000;

SerialComm::SerialComm()
{
}

SerialComm::~SerialComm()
{
}

void SerialComm::init()
{
    Serial.setRxBufferSize(262144);

    // serial delay
    Serial.begin(921600);
    delay(500);
}

bool SerialComm::connected()
{
    return (lastPackageReceived > millis());
}

size_t SerialComm::read_raw(char *buffer, unsigned int len)
{
    if (Serial.available() > 0)
    {
        lastPackageReceived = millis() + USB_TIMEOUT;
        return Serial.read(buffer, len);
    }
    else
    {
        return 0;
    }
}

size_t SerialComm::write_raw(char *buffer, unsigned int len)
{
    return Serial.write(buffer, len);
}

unsigned long SerialComm::last_package_received()
{
    return lastPackageReceived;
}