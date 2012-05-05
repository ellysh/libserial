#ifndef DEBUG_CLIENT_H
#define DEBUG_CLIENT_H

#include <string>

namespace serial
{

class DebugClient
{
public:
    virtual ~DebugClient() {};

    void SetLogFile(std::string log_file);
};

}

#endif
