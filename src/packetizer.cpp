#include "packetizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <memory.h>
#include <regex>

void Packetizer::append_to_backlog(char *buffer, unsigned int len)
{
    backlog.append(buffer, len);
}

// if the connection breaks, we will clear the backlog
void Packetizer::clear_backlog()
{
    in_error_state = false;
    backlog = std::string();
}

std::tuple<uint8_t, std::string> Packetizer::next_packet() // can be of length 0, than we have to ignore it
{

    if (in_error_state)
    {
        auto found = backlog.find("ADAD"); // look for stopbit, ignore incomplete packet
        if (found != std::string::npos)
        {
            in_error_state = false;
            backlog.erase(0, found + 4);
        }
        return std::make_tuple(0, std::string());
    }
    if (backlog.length() >= 1 && last_command == 0)
    {
        last_command = backlog[0];
        backlog.erase(0, 1);
        // since there are commands, that don't need a payload at all,
        // we will return here
        if (payloadless(last_command))
        {
            auto retval = std::make_tuple(last_command, std::string());
            last_command = 0;
            return retval;
        }
    }
    if (backlog.length() >= 3)
    {
        uint_fast32_t number = uint_fast32_t((unsigned char)(backlog[2]) << 16 |
                                             (unsigned char)(backlog[1]) << 8 |
                                             (unsigned char)(backlog[0]));

        if ((backlog.length() - 3) >= number)
        {
            std::string buff = backlog.substr(3, number);
            if (check_start_bytes(buff) && check_stop_bytes(buff) && number <= 320 * 170 * 3 && number > 0)
            {
                backlog.erase(0, number + 3);
                std::string escaped = buff.substr(4, number - 8);
                replace(escaped, "DD", "D");
                replace(escaped, "AA", "A");
                auto retval = std::make_tuple(last_command, escaped);
                last_command = 0;
                return retval;
            }
            else
            {
                in_error_state = true;
                last_command = 0;
            }
        }
    }
    return std::make_tuple(0, std::string());
}

uint_fast8_t Packetizer::next_command()
{
    return last_command;
}

bool Packetizer::check_start_bytes(std::string packet)
{
    return packet.compare(0, 4, "DADA") == 0;
}

bool Packetizer::check_stop_bytes(std::string packet)
{
    return packet.compare(packet.length() - 4, 4, "ADAD") == 0;
}

void Packetizer::replace(std::string &str, std::string substring, std::string replaceable)
{
    size_t pos = 0;
    while ((pos = str.find(substring, pos)) != std::string::npos)
    {
        str.replace(pos, substring.length(), replaceable);
        pos += replaceable.length();
    }
}

bool Packetizer::payloadless(uint_fast8_t command)
{
    return command != 0 && command != 228 && command != 20;
}
