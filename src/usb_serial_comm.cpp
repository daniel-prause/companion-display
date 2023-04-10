#include "usb_serial_comm.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <algorithm>
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

// waits for data to be available
int USBSerialComm::readCommand()
{
    if (USBSerial.available() > 0)
    {
        return USBSerial.read();
    }
    else
    {
        return 0; // noop command, since there was no message
    }
}

uint_fast32_t USBSerialComm::readPayloadLength()
{
    int startBytes = 0;
    while (USBSerial.available() > 0)
    {
        char length[4] = "";
        size_t readLength = USBSerial.readBytes(length, 4);
        startBytes += readLength;
        if (startBytes == 4)
        {
            uint_fast32_t number = 0;
            memcpy(&number, &length, 4);
            return number;
        }
    }
    return 0;
}

// returns the length of the received bytes
std::tuple<uint_fast32_t, char *> USBSerialComm::readPayload()
{
    uint_fast32_t length = readPayloadLength();
    const auto noValue = std::make_tuple(0, (char *)NULL);
    // return if length is zero; that means that something went wrong oO!
    if (length == 0)
    {
        return noValue;
    }
    // check, if there are bytes in the queue
    size_t totalBytesRead = 0;
    while (USBSerial.available() > 0)
    {
        lastPackageReceived = millis() + USB_TIMEOUT;
        // reallocate buffer, if it needs to be bigger
        if (length > currentBufferLength)
        {
            char *tmp = (char *)realloc(buffer, length);
            if (tmp != NULL)
            {
                buffer = tmp;
                currentBufferLength = length;
            }
            else
            {
                return noValue;
            }
        }

        if (totalBytesRead < length)
        {
            size_t currentBytesRead = USBSerial.readBytes(buffer + totalBytesRead, std::min<unsigned int>(length - totalBytesRead, 8192));
            totalBytesRead += currentBytesRead;
        }
        else
        {
            return std::make_tuple(totalBytesRead, buffer);
        }
    }

    if (totalBytesRead == length)
    {
        return std::make_tuple(totalBytesRead, buffer);
    }
    else
    {
        return noValue;
    }
}

bool USBSerialComm::connected()
{
    return (lastPackageReceived > millis());
}

size_t USBSerialComm::read_raw(char *buffer, unsigned int len)
{
    if (USBSerial.available() > 0)
    {
        return USBSerial.readBytes(buffer, len);
    }
    else
    {
        return 0;
    }
}