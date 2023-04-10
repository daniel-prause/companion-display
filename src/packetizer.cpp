#include "packetizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <memory.h>

void Packetizer::append_to_backlog(char *buffer, unsigned int len)
{
    backlog.append(buffer, len);
}

std::string Packetizer::next_packet() // can be of length 0, than we have to ignore it
{
    if (backlog.length() >= 4)
    {
        uint_fast32_t number = 0;
        memcpy(&number, backlog.c_str(), 4);

        if ((backlog.length() - 4) >= number)
        {
            std::string buff = backlog.substr(4, number);
            backlog.erase(0, number + 4);
            return buff;
        }
    }
    return std::string();
}
