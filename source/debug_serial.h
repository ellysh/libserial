#ifndef DEBUG_SERIAL_H
#define DEBUG_SERIAL_H

#include <fstream>

#include <types_serial.h>

namespace serial
{

class Debug
{
public:
    Debug(std::string log_file);
    ~Debug();

    std::ostream& Log();
    void Log(const char* fmt, ...);
    void LogByteArray(std::ostream& stream, const ByteArray& byte_array);

private:
    std::string log_file_;
    std::ofstream file_;
};

}

#endif
