#include "usb_serial_comm.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <algorithm>
#define USB_TIMEOUT 2000;

USBSerialComm::USBSerialComm()
{
}

USBSerialComm::~USBSerialComm()
{
    free(buffer);
}

void USBSerialComm::init()
{
    USB.begin();
    USBSerial.setRxBufferSize(262144);
    USBSerial.begin(921600);
}

bool USBSerialComm::connected()
{
    return (lastPackageReceived > millis());
}

size_t USBSerialComm::read_raw(char *buffer, unsigned int len)
{
    if (USBSerial.available() > 0)
    {
        lastPackageReceived = millis() + USB_TIMEOUT;
        return USBSerial.readBytes(buffer, len);
    }
    else
    {
        return 0;
    }
}