#ifndef DEBUG_H
#define DEBUG_H

#include <fstream>

#include "types_serial.h"

namespace serial
{

//#define __DEBUG__

#define GET_DEBUG() \
    Debug* debug = Debug::Instance(); \

class Debug
{
public:
    static Debug* Instance();
    static void SetLogFile(std::string log_file);

    ~Debug();

    std::ostream& Log();
    void Log(const char* fmt, ...);
    void LogByteArray(std::ostream& stream, const ByteArray& byte_array);

private:
    static Debug* instance_;
    static std::string log_file_;

    std::ofstream file_;

    Debug();
};

}

#endif
