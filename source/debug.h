#ifndef DEBUG_H
#define DEBUG_H

#include <fstream>

namespace planar
{

//#define __DEBUG__

#define GET_DEBUG() \
    Debug* debug = Debug::Instance(); \

class Debug
{
public:
    static Debug* Instance();
    ~Debug();

    std::ostream& Log();
    void Log(const char* fmt, ...);

private:
    static Debug* instance_;

    std::ofstream file_;

    Debug();
};

}

#endif
