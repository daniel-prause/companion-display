#include <string>
#include <queue>
#include <memory>
class Packetizer
{

public:
    Packetizer(){};
    void append_to_backlog(char *buffer, unsigned int len);
    std::string next_packet();

private:
    std::string backlog;
};