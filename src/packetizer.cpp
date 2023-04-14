#include "packetizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <memory.h>

void Packetizer::append_to_backlog(char *buffer, unsigned int len)
{
    backlog.append(buffer, len);
}

// if the connection breaks, we will clear the backlog
void Packetizer::clear_backlog()
{
    backlog = std::string();
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
            if (check_start_bytes(buff) && check_stop_bytes(buff))
            {
                backlog.erase(0, number + 4);
                return buff.substr(4, buff.length() - 4);
            }
            else
            {
                auto found = backlog.find("ADAD"); // look for stopbit, ignore incomplete packet
                if (found != std::string::npos)
                {
                    backlog.erase(0, found + 4);
                }
                else
                {
                    // erase everything
                    backlog = std::string();
                }
            }
        }
    }
    return std::string();
}

bool Packetizer::check_start_bytes(std::string packet)
{
    if (packet.substr(0, 4) == "DADA")
    {
        return true;
    }
    return false;
}

bool Packetizer::check_stop_bytes(std::string packet)
{
    if (packet.substr(packet.length() - 4, 4) == "ADAD")
    {
        return true;
    }
    return false;
}
