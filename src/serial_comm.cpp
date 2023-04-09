#include "serial_comm.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <algorithm>
SerialComm::SerialComm()
{
}

SerialComm::~SerialComm()
{
    free(buffer);
}

void SerialComm::init()
{
    Serial.begin(3000000);
    Serial.setRxBufferSize(1024 * 1024);
    Serial.setTimeout(1000);
}

// waits for data to be available
int SerialComm::readCommand()
{
    if (Serial.available() > 0)
    {
        return Serial.read();
    }
    else
    {
        return 0; // noop command, since there was no message
    }
}

uint_fast32_t SerialComm::readPayloadLength()
{
    if (Serial.available() > 0)
    {
        char length[9] = "";
        size_t readLength = Serial.readBytes(length, 8);
        if (readLength < 8)
        {
            return 0;
        }
        uint_fast32_t number = (uint_fast32_t)strtoul(length, NULL, 0);
        return number;
    }
    return 0;
}

// returns the length of the received bytes
std::tuple<uint_fast32_t, char *> SerialComm::readPayload()
{
    uint_fast32_t length = readPayloadLength();
    auto noValue = std::make_tuple(0, (char *)NULL);
    // return if length is zero; that means that something went wrong oO!
    if (length == 0)
    {
        return noValue;
    }
    // check, if there are bytes in the queue
    size_t totalBytesRead = 0;
    while (Serial.available() > 0)
    {
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
            char byteBuffer[1024] = "";
            size_t currentBytesRead = Serial.readBytes(byteBuffer, std::min((int)length - (int)totalBytesRead, (int)sizeof(byteBuffer)));
            memcpy(buffer + totalBytesRead, byteBuffer, currentBytesRead);
            totalBytesRead += currentBytesRead;
        }
        else
        {
            return std::make_tuple(totalBytesRead, buffer);
        }
    }
    return std::make_tuple(totalBytesRead, buffer);
}
