#include <string>
#include <queue>
#include <memory>
class Packetizer
{

public:
    Packetizer(){};
    void append_to_backlog(char *buffer, unsigned int len);
    void clear_backlog();
    std::tuple<uint8_t, std::string> next_packet();
    uint8_t next_command();

private:
    std::string backlog;
    bool check_start_bytes(std::string);
    bool check_stop_bytes(std::string);
    void replace(std::string &str, std::string substring, std::string replaceable);
    bool in_error_state = false;
    uint8_t last_command = 0;
};