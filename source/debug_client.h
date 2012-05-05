#ifndef DEBUG_CLIENT_H
#define DEBUG_CLIENT_H

#include <string>

namespace serial
{

class Debug;

class DebugClient
{
public:
    DebugClient(std::string log_file);
    virtual ~DebugClient();

protected:
    Debug* debug_;
};

}

#endif
